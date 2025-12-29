#include "map.hh"
#include <iomanip> 
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <chrono>
#ifdef _WIN32
#include <windows.h>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////
//to_hex
//convert int to hex string, apply zero padding
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string to_hex(int n)
{
  std::stringstream ss;
  ss << std::hex << std::setw(2) << std::setfill('0') << n; // Use std::setw and std::setfill for zero padding
  return ss.str();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//rgb_to_hex
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string rgb_to_hex(int r, int g, int b)
{
  std::string str("#");
  str += to_hex(r);
  str += to_hex(g);
  str += to_hex(b);
  return str;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// load_geojson
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string load_geojson(const std::string& name)
{
  std::ifstream file(name);
  if (!file.is_open())
  {
    return "";
  }
  auto start_time = std::chrono::high_resolution_clock::now();
  std::string str;
  std::string line;
  while (std::getline(file, line))
  {
    str += line;
  }
  file.close();
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
  return str;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// escape_js_string
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string escape_js_string(const std::string& input)
{
  std::string output;
  output.reserve(input.size());
  for (size_t i = 0; i < input.size(); ++i)
  {
    char c = input[i];
    switch (c)
    {
    case '\'': output += "\\'"; break;
    case '\"': output += "\\\""; break;
    case '\\': output += "\\\\"; break;
    case '\n': output += "\\n"; break;
    case '\r': output += "\\r"; break;
    case '\t': output += "\\t"; break;
    default: output += c; break;
    }
  }
  return output;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// WMapLibre
/////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Wt
{
  class WMapLibre::Impl : public WWebWidget
  {
  public:
    Impl();
    virtual DomElementType domElementType() const override;
  };

  WMapLibre::Impl::Impl()
  {
    setInline(false);
  }

  DomElementType WMapLibre::Impl::domElementType() const
  {
    return DomElementType::DIV;
  }

  WMapLibre::WMapLibre()
  {
    setImplementation(std::unique_ptr<Impl>(impl = new Impl()));
    WApplication* app = WApplication::instance();
    this->addCssRule("body", "margin: 0; padding: 0;");
    this->addCssRule("#" + id(), "position: absolute; top: 0; bottom: 0; width: 100%;");
    app->useStyleSheet("https://unpkg.com/maplibre-gl@4.7.1/dist/maplibre-gl.css");
    const std::string library = "https://unpkg.com/maplibre-gl@4.7.1/dist/maplibre-gl.js";
    app->require(library, "maplibre");
  }

  WMapLibre::~WMapLibre()
  {
    coordinates.clear();
    geojson.clear();
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // render
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  void WMapLibre::render(WFlags<RenderFlag> flags)
  {
    WCompositeWidget::render(flags);

    std::vector<std::string> ward_color =
    { rgb_to_hex(128, 128, 0), //olive
      rgb_to_hex(255, 255, 0), //yellow 
      rgb_to_hex(0, 128, 0), //green
      rgb_to_hex(0, 255, 0), //lime
      rgb_to_hex(0, 128, 128), //teal
      rgb_to_hex(0, 255, 255), //aqua
      rgb_to_hex(0, 0, 255), //blue
      rgb_to_hex(128, 0, 128) //purple
    };

    std::map<std::string, std::string> colors;
    colors["Abandoned Vehicle"] = "#8B4513";      // Brown 
    colors["Bulk Collection"] = "#00FF00";        // Green
    colors["Illegal Dumping"] = "#0000FF";        // Blue
    colors["Pothole"] = "#FFFF00";                // Yellow
    colors["Streetlight Repair"] = "#FF00FF";     // Magenta
    colors["Tree Inspection"] = "#00FFFF";        // Cyan
    colors["Parking Enforcement"] = "#FFA500";    // Orange
    colors["Graffiti Removal"] = "#800080";       // Purple
    colors["Trash Collection"] = "#FFC0CB";       // Pink
    colors["Rodent Inspection"] = "#FF0000";      // Red 

    if (flags.test(RenderFlag::Full))
    {
      std::stringstream js;

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      // create map
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      js << "const map = new maplibregl.Map({\n"
        << "  container: " << jsRef() << ",\n"
        << "  style: 'https://basemaps.cartocdn.com/gl/positron-gl-style/style.json',\n"
        << "  center: [-77.0369, 38.9072],\n"
        << "  zoom: 12\n"
        << "});\n"

        << "map.addControl(new maplibregl.NavigationControl());\n";

#ifdef _WIN32
      OutputDebugStringA(js.str().c_str());
#endif

      js << "map.on('load', function() {\n";

      js << "var ward_color = [";
      for (size_t idx = 0; idx < ward_color.size(); ++idx)
      {
        js << "'" << ward_color[idx] << "'";
        if (idx < ward_color.size() - 1) js << ",";
      }
      js << "];\n";

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      // add ward layer
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      js << "map.addSource('wards', {\n"
        << "  'type': 'geojson',\n"
        << "  'data': " << geojson << "\n"
        << "});\n"

        << "map.addLayer({\n"
        << "  'id': 'wards-fill',\n"
        << "  'type': 'fill',\n"
        << "  'source': 'wards',\n"
        << "  'paint': {\n"
        << "    'fill-color': ['get', ['to-string', ['get', 'WARD']], ['literal', {\n";

      for (size_t idx = 0; idx < ward_color.size(); ++idx)
      {
        js << "      '" << (idx + 1) << "': '" << ward_color[idx] << "'";
        if (idx < ward_color.size() - 1) js << ",\n";
      }

      js << "\n    }]],\n"
        << "    'fill-opacity': 0.2\n"
        << "  }\n"
        << "});\n";

#ifdef _WIN32
      OutputDebugStringA(js.str().c_str());
#endif

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      // create separate layer for each service type
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      if (!coordinates.empty())
      {
        js << "var all_features = [];\n"
          << "var layer_ids = [];\n";

        std::map<std::string, std::vector<MapCoordinate>> by_service;

        for (size_t idx = 0; idx < coordinates.size(); ++idx)
        {
          std::string service = coordinates[idx].SERVICECODEDESCRIPTION;
          by_service[service].push_back(coordinates[idx]);
        }

        std::map<std::string, std::vector<MapCoordinate>>::iterator service_it;
        for (service_it = by_service.begin(); service_it != by_service.end(); ++service_it)
        {
          const std::string& service = service_it->first;
          const std::vector<MapCoordinate>& coords = service_it->second;

          std::string color = "#ff0000";
          std::map<std::string, std::string>::iterator color_it = colors.find(service);
          if (color_it != colors.end())
          {
            color = color_it->second;
          }

          std::string service_ = service;
          for (size_t i = 0; i < service_.length(); ++i)
          {
            if (service_[i] == ' ') service_[i] = '_';
          }

          std::string source_id = "incidents-" + service_;
          std::string layer_id = "incidents-layer-" + service_;

          js << "var features_" << service_ << " = [];\n";

          for (size_t idx = 0; idx < coords.size(); ++idx)
          {
            std::string latitude = coords[idx].LATITUDE;
            std::string longitude = coords[idx].LONGITUDE;
            std::string address = escape_js_string(coords[idx].STREETADDRESS);
            std::string date = escape_js_string(coords[idx].ADDDATE);

            if (!latitude.empty() && !longitude.empty())
            {
              js << "features_" << service_ << ".push({"
                << "'type': 'Feature',"
                << "'geometry': {"
                << "'type': 'Point',"
                << "'coordinates': [" << longitude << ", " << latitude << "]"
                << "},"
                << "'properties': {"
                << "'service': '" << service << "',"
                << "'address': '" << address << "',"
                << "'date': '" << date << "'"
                << "}"
                << "});\n";
            }
          }

          js << "all_features = all_features.concat(features_" << service_ << ");\n";

          js << "map.addSource('" << source_id << "', {\n"
            << "  'type': 'geojson',\n"
            << "  'data': {\n"
            << "    'type': 'FeatureCollection',\n"
            << "    'features': features_" << service_ << "\n"
            << "  }\n"
            << "});\n";

          js << "map.addLayer({\n"
            << "  'id': '" << layer_id << "',\n"
            << "  'type': 'circle',\n"
            << "  'source': '" << source_id << "',\n"
            << "  'paint': {\n"
            << "    'circle-radius': [\n"
            << "      'interpolate', ['linear'], ['zoom'],\n"
            << "      10, 3,\n"
            << "      12, 5,\n"
            << "      14, 7,\n"
            << "      16, 9\n"
            << "    ],\n"
            << "    'circle-color': '" << color << "',\n"
            << "    'circle-opacity': 0.2\n"
            << "  }\n"
            << "});\n";

          js << "var popup_" << service_ << " = new maplibregl.Popup({\n"
            << "  closeButton: false,\n"
            << "  closeOnClick: false\n"
            << "});\n";

          js << "map.on('mousemove', '" << layer_id << "', function(e) {\n"
            << "  map.getCanvas().style.cursor = 'pointer';\n"
            << "  var coordinates = e.features[0].geometry.coordinates.slice();\n"
            << "  var service = e.features[0].properties.service;\n"
            << "  var address = e.features[0].properties.address || 'N/A';\n"
            << "  var date = e.features[0].properties.date || 'N/A';\n"
            << "  var html = '<strong>' + service + '</strong><br>' + address + '<br>' + date;\n"
            << "  while (Math.abs(e.lngLat.lng - coordinates[0]) > 180) {\n"
            << "    coordinates[0] += e.lngLat.lng > coordinates[0] ? 360 : -360;\n"
            << "  }\n"
            << "  popup_" << service_ << ".setLngLat(coordinates).setHTML(html).addTo(map);\n"
            << "});\n";

          js << "map.on('mouseleave', '" << layer_id << "', function() {\n"
            << "  map.getCanvas().style.cursor = '';\n"
            << "  popup_" << service_ << ".remove();\n"
            << "});\n";

          js << "layer_ids.push('" << layer_id << "');\n";

        }
      }

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      // rectangle selection
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      if (!coordinates.empty())
      {
        js << "\n"
          << "var container = map.getContainer();\n"
          << "var start = null;\n"
          << "var current = null;\n"
          << "var box = null;\n"
          << "var is_drawing = false;\n"
          << "\n"

          /////////////////////////////////////////////////////////////////////////////////////////////////////
          // remove_box
          /////////////////////////////////////////////////////////////////////////////////////////////////////

          << "function remove_box() {\n"
          << "  if (box && box.parentNode) {\n"
          << "    box.parentNode.removeChild(box);\n"
          << "  }\n"
          << "  box = null;\n"
          << "}\n"
          << "\n"
          << "function create_box() {\n"
          << "  remove_box();\n"
          << "  box = document.createElement('div');\n"
          << "  box.style.position = 'absolute';\n"
          << "  box.style.background = 'rgba(56, 135, 190, 0.1)';\n"
          << "  box.style.border = '2px dashed #3887be';\n"
          << "  box.style.pointerEvents = 'none';\n"
          << "  box.style.zIndex = '1000';\n"
          << "  container.appendChild(box);\n"
          << "}\n"
          << "\n"

          /////////////////////////////////////////////////////////////////////////////////////////////////////
          // update_box
          /////////////////////////////////////////////////////////////////////////////////////////////////////

          << "function update_box(e) {\n"
          << "  if (!box) return;\n"
          << "  var rect = container.getBoundingClientRect();\n"
          << "  var min_x = Math.min(start.x, e.clientX) - rect.left;\n"
          << "  var max_x = Math.max(start.x, e.clientX) - rect.left;\n"
          << "  var min_y = Math.min(start.y, e.clientY) - rect.top;\n"
          << "  var max_y = Math.max(start.y, e.clientY) - rect.top;\n"
          << "  box.style.left = min_x + 'px';\n"
          << "  box.style.top = min_y + 'px';\n"
          << "  box.style.width = (max_x - min_x) + 'px';\n"
          << "  box.style.height = (max_y - min_y) + 'px';\n"
          << "}\n"
          << "\n"
          << "container.addEventListener('mousedown', function(e) {\n"
          << "  if (!e.shiftKey) return;\n"
          << "  e.preventDefault();\n"
          << "  is_drawing = true;\n"
          << "  start = {x: e.clientX, y: e.clientY};\n"
          << "  create_box();\n"
          << "  map.dragPan.disable();\n"
          << "});\n"
          << "\n"
          << "document.addEventListener('mousemove', function(e) {\n"
          << "  if (!is_drawing) return;\n"
          << "  e.preventDefault();\n"
          << "  current = {x: e.clientX, y: e.clientY};\n"
          << "  update_box(e);\n"
          << "});\n"
          << "\n"
          << "document.addEventListener('mouseup', function(e) {\n"
          << "  if (!is_drawing) return;\n"
          << "  is_drawing = false;\n"
          << "  map.dragPan.enable();\n"
          << "  \n"
          << "  if (!current || !start) {\n"
          << "    remove_box();\n"
          << "    return;\n"
          << "  }\n"
          << "  \n"
          << "  var width = Math.abs(current.x - start.x);\n"
          << "  var height = Math.abs(current.y - start.y);\n"
          << "  \n"
          << "  if (width < 5 || height < 5) {\n"
          << "    remove_box();\n"
          << "    start = null;\n"
          << "    current = null;\n"
          << "    return;\n"
          << "  }\n"
          << "  \n"
          << "  var rect = container.getBoundingClientRect();\n"
          << "  var start_x = start.x - rect.left;\n"
          << "  var start_y = start.y - rect.top;\n"
          << "  var current_x = current.x - rect.left;\n"
          << "  var current_y = current.y - rect.top;\n"
          << "  \n"
          << "  var start_point = map.unproject([start_x, start_y]);\n"
          << "  var end_point = map.unproject([current_x, current_y]);\n"
          << "  \n"
          << "  var bbox = [\n"
          << "    Math.min(start_point.lng, end_point.lng),\n"
          << "    Math.min(start_point.lat, end_point.lat),\n"
          << "    Math.max(start_point.lng, end_point.lng),\n"
          << "    Math.max(start_point.lat, end_point.lat)\n"
          << "  ];\n"
          << "  \n"
          << "  console.log('BBox:', bbox);\n"
          << "  filter_by_bbox(bbox);\n"
          << "  remove_box();\n"
          << "  start = null;\n"
          << "  current = null;\n"
          << "});\n"
          << "\n"

          /////////////////////////////////////////////////////////////////////////////////////////////////////
          // filter_by_bbox
          /////////////////////////////////////////////////////////////////////////////////////////////////////

          << "function filter_by_bbox(bbox) {\n"
          << "  var filtered = all_features.filter(function(f) {\n"
          << "    if (!f.geometry || !f.geometry.coordinates) return false;\n"
          << "    var c = f.geometry.coordinates;\n"
          << "    return c[0] >= bbox[0] && c[0] <= bbox[2] && c[1] >= bbox[1] && c[1] <= bbox[3];\n"
          << "  });\n"
          << "  \n"
          << "  var by_service = {};\n"
          << "  filtered.forEach(function(f) {\n"
          << "    var service = f.properties.service;\n"
          << "    if (!by_service[service]) by_service[service] = [];\n"
          << "    by_service[service].push(f);\n"
          << "  });\n"
          << "  \n"
          << "  layer_ids.forEach(function(layer_id) {\n"
          << "    var source_id = layer_id.replace('incidents-layer-', 'incidents-');\n"
          << "    var service = layer_id.replace('incidents-layer-', '').replace(/_/g, ' ');\n"
          << "    var features = by_service[service] || [];\n"
          << "    var source = map.getSource(source_id);\n"
          << "    if (source) {\n"
          << "      source.setData({\n"
          << "        'type': 'FeatureCollection',\n"
          << "        'features': features\n"
          << "      });\n"
          << "    }\n"
          << "  });\n"
          << "}\n"
          << "\n"
          << "window.clearSelection = function() {\n"
          << "  var by_service = {};\n"
          << "  all_features.forEach(function(f) {\n"
          << "    var service = f.properties.service;\n"
          << "    if (!by_service[service]) by_service[service] = [];\n"
          << "    by_service[service].push(f);\n"
          << "  });\n"
          << "  \n"
          << "  layer_ids.forEach(function(layer_id) {\n"
          << "    var source_id = layer_id.replace('incidents-layer-', 'incidents-');\n"
          << "    var service = layer_id.replace('incidents-layer-', '').replace(/_/g, ' ');\n"
          << "    var features = by_service[service] || [];\n"
          << "    var source = map.getSource(source_id);\n"
          << "    if (source) {\n"
          << "      source.setData({\n"
          << "        'type': 'FeatureCollection',\n"
          << "        'features': features\n"
          << "      });\n"
          << "    }\n"
          << "  });\n"
          << "  \n"
          << "};\n"
          << "\n";
      }

      //close map.on('load')
      js << "});\n";

      WApplication* app = WApplication::instance();
      app->doJavaScript(js.str());
    }
  }

}// namespace Wt

