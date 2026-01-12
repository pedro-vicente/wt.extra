# wt.extra
Wt framework with web maps

[Wt](https://www.webtoolkit.eu/wt) is a C++ library for developing web applications. 

Wt.extra adds use of several popular Javascript libraries, including Mapbox

[Mapbox](https://www.mapbox.com/) is a Javascript library for developing web mapping applications. 


# Live demo at

https://pedro-vicente.net:9444/

# DC 311 Mapping

![image](https://github.com/user-attachments/assets/ca689684-3ce3-4073-b51e-dae85645095c)

This project creates an interactive web map visualization of Washington DC's 311 service request data using the Wt C++ web framework and Mapbox mapping technology.


# Usage 

Build Boost and Wt on a bash shell (Linux, Mac, Windows) with

```bash
./build.boost.sh 
./build.wt.sh 
```

Build with

```bash
./build.cmake.sh 
```

Access the web interface at `http://localhost:8080`

## Features

- **Interactive Map**: Browse DC 311 service requests on an interactive Mapbox map
- **Dual CSV Loading**: Combines multiple CSV files for comprehensive data coverage
- **Service Categorization**: Color-coded markers by service type (potholes, street lights, trash, etc.)
- **Ward-based Filtering**: Filter requests by DC's 8 political wards
- **Real-time Statistics**: Dashboard showing request counts and distributions

## Data Structure

The application processes DC 311 service request CSV files with 37 columns of data including:

- **Geographic Data**: Latitude/longitude coordinates for mapping
- **Service Classification**: Request types, responsible agencies, priority levels
- **Status Tracking**: Request lifecycle from submission to resolution
- **Address Information**: Street addresses, wards, zip codes

**For detailed information about the CSV data structure and service codes, see [DC311.md](DC311.md)**

## Data Processing

The application automatically:
- Loads and combines multiple CSV files
- Validates geographic coordinates for DC area
- Categorizes service requests by type and agency
- Generates statistics by ward and service category
- Prepares data for interactive map visualization

## Reference

Compare with the official DC 311 mapping service: https://octo.dc.gov/service/dc-311-mapping
