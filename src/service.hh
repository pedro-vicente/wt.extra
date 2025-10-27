#ifndef SERVICE_REQUEST_HH
#define SERVICE_REQUEST_HH

#include <string>

/////////////////////////////////////////////////////////////////////////////////////////////////////
// ServiceRequest - Dbo mapping class using CSV field names
// CSV Header: X,Y,SERVICEREQUESTID,STREETADDRESS,CITY,STATE,ZIPCODE,WARD,
//             SERVICECODE,SERVICECODEDESCRIPTION,SERVICETYPECODEDESCRIPTION,
//             ORGANIZATIONACRONYM,SERVICECALLCOUNT,ADDDATE,RESOLUTIONDATE,
//             SERVICEDUEDATE,SERVICEORDERDATE,STATUS_CODE,SERVICEORDERSTATUS,
//             INSPECTIONFLAG,INSPECTIONDATE,INSPECTORNAME,PRIORITY,DETAILS,
//             XCOORD,YCOORD,LATITUDE,LONGITUDE,MARADDRESSREPOSITORYID,GIS_ID,
//             GLOBALID,CREATED,EDITED,GDB_FROM_DATE,GDB_TO_DATE,GDB_ARCHIVE_OID,
//             SE_ANNO_CAD_DATA,OBJECTID
/////////////////////////////////////////////////////////////////////////////////////////////////////

class ServiceRequest
{
public:

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // 0-1: Coordinate fields
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::string X;
  std::string Y;

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // 2: Service request identification
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::string SERVICEREQUESTID;

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // 3-7: Address information
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::string STREETADDRESS;
  std::string CITY;
  std::string STATE;
  std::string ZIPCODE;
  std::string WARD;

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // 8-12: Service information
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::string SERVICECODE;
  std::string SERVICECODEDESCRIPTION;
  std::string SERVICETYPECODEDESCRIPTION;
  std::string ORGANIZATIONACRONYM;
  std::string SERVICECALLCOUNT;

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // 13-16: Date fields
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::string ADDDATE;
  std::string RESOLUTIONDATE;
  std::string SERVICEDUEDATE;
  std::string SERVICEORDERDATE;

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // 17-18: Status fields
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::string STATUS_CODE;
  std::string SERVICEORDERSTATUS;

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // 19-21: Inspection fields
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::string INSPECTIONFLAG;
  std::string INSPECTIONDATE;
  std::string INSPECTORNAME;

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // 22-23: Additional fields
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::string PRIORITY;
  std::string DETAILS;

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // 24-27: Additional coordinate fields
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::string XCOORD;
  std::string YCOORD;
  std::string LATITUDE;
  std::string LONGITUDE;

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // 28-37: System fields
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::string MARADDRESSREPOSITORYID;
  std::string GIS_ID;
  std::string GLOBALID;
  std::string CREATED;
  std::string EDITED;
  std::string GDB_FROM_DATE;
  std::string GDB_TO_DATE;
  std::string GDB_ARCHIVE_OID;
  std::string SE_ANNO_CAD_DATA;
  std::string OBJECTID;

  template<class Action>
  void persist(Action& a)
  {
    Wt::Dbo::field(a, X, "X");
    Wt::Dbo::field(a, Y, "Y");
    Wt::Dbo::field(a, SERVICEREQUESTID, "SERVICEREQUESTID");
    Wt::Dbo::field(a, STREETADDRESS, "STREETADDRESS");
    Wt::Dbo::field(a, CITY, "CITY");
    Wt::Dbo::field(a, STATE, "STATE");
    Wt::Dbo::field(a, ZIPCODE, "ZIPCODE");
    Wt::Dbo::field(a, WARD, "WARD");
    Wt::Dbo::field(a, SERVICECODE, "SERVICECODE");
    Wt::Dbo::field(a, SERVICECODEDESCRIPTION, "SERVICECODEDESCRIPTION");
    Wt::Dbo::field(a, SERVICETYPECODEDESCRIPTION, "SERVICETYPECODEDESCRIPTION");
    Wt::Dbo::field(a, ORGANIZATIONACRONYM, "ORGANIZATIONACRONYM");
    Wt::Dbo::field(a, SERVICECALLCOUNT, "SERVICECALLCOUNT");
    Wt::Dbo::field(a, ADDDATE, "ADDDATE");
    Wt::Dbo::field(a, RESOLUTIONDATE, "RESOLUTIONDATE");
    Wt::Dbo::field(a, SERVICEDUEDATE, "SERVICEDUEDATE");
    Wt::Dbo::field(a, SERVICEORDERDATE, "SERVICEORDERDATE");
    Wt::Dbo::field(a, STATUS_CODE, "STATUS_CODE");
    Wt::Dbo::field(a, SERVICEORDERSTATUS, "SERVICEORDERSTATUS");
    Wt::Dbo::field(a, INSPECTIONFLAG, "INSPECTIONFLAG");
    Wt::Dbo::field(a, INSPECTIONDATE, "INSPECTIONDATE");
    Wt::Dbo::field(a, INSPECTORNAME, "INSPECTORNAME");
    Wt::Dbo::field(a, PRIORITY, "PRIORITY");
    Wt::Dbo::field(a, DETAILS, "DETAILS");
    Wt::Dbo::field(a, XCOORD, "XCOORD");
    Wt::Dbo::field(a, YCOORD, "YCOORD");
    Wt::Dbo::field(a, LATITUDE, "LATITUDE");
    Wt::Dbo::field(a, LONGITUDE, "LONGITUDE");
    Wt::Dbo::field(a, MARADDRESSREPOSITORYID, "MARADDRESSREPOSITORYID");
    Wt::Dbo::field(a, GIS_ID, "GIS_ID");
    Wt::Dbo::field(a, GLOBALID, "GLOBALID");
    Wt::Dbo::field(a, CREATED, "CREATED");
    Wt::Dbo::field(a, EDITED, "EDITED");
    Wt::Dbo::field(a, GDB_FROM_DATE, "GDB_FROM_DATE");
    Wt::Dbo::field(a, GDB_TO_DATE, "GDB_TO_DATE");
    Wt::Dbo::field(a, GDB_ARCHIVE_OID, "GDB_ARCHIVE_OID");
    Wt::Dbo::field(a, SE_ANNO_CAD_DATA, "SE_ANNO_CAD_DATA");
    Wt::Dbo::field(a, OBJECTID, "OBJECTID");
  }
};

#endif
