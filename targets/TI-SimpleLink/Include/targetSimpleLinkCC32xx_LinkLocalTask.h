//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Texas Instruments Incorporated.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _SIMPLELINK_CC32XX_LINKLOCAL_TASK_H_
#define _SIMPLELINK_CC32XX_LINKLOCAL_TASK_H_

/* TI-DRIVERS Header files */
#include <ti/drivers/net/wifi/simplelink.h>

/* POSIX Header files */
#include <mqueue.h>
#include <semaphore.h>

#define OOB_IS_NETAPP_MORE_DATA(flags)              ((flags & \
                                                      SL_NETAPP_REQUEST_RESPONSE_FLAGS_CONTINUATION) \
                                                     == \
                                                     SL_NETAPP_REQUEST_RESPONSE_FLAGS_CONTINUATION)
#define OOB_IS_NETAPP_ERROR(flags)                  ((flags & \
                                                      SL_NETAPP_REQUEST_RESPONSE_FLAGS_ERROR) \
                                                     == \
                                                     SL_NETAPP_REQUEST_RESPONSE_FLAGS_ERROR)

/* header fields definitions */
#define WEB_SERVER_VERSION                      "HTTP Version:"
#define WEB_SERVER_REQUEST_URI                  "Request URI:"
#define WEB_SERVER_QUERY_STRING                 "Query String:"
#define WEB_SERVER_HEADER_CONTENT_TYPE          "Content-Type: "
#define WEB_SERVER_HEADER_CONTENT_LEN           "Content-Length: "
#define WEB_SERVER_HEADER_LOCATION              "Location: "
#define WEB_SERVER_HEADER_SERVER                "Server: "
#define WEB_SERVER_HEADER_USER_AGENT            "User-Agent: "
#define WEB_SERVER_HEADER_COOKIE                "Cookie:"
#define WEB_SERVER_HEADER_SET_COOKIE            "Set-Cookie: "
#define WEB_SERVER_HEADER_UPGRADE               "Upgrade: "
#define WEB_SERVER_HEADER_REFERER               "Referer: "
#define WEB_SERVER_HEADER_ACCEPT                "Accept: "
#define WEB_SERVER_HEADER_CONTENT_ENCODING      "Content-Encoding: "
#define WEB_SERVER_HEADER_CONTENT_DISPOSITION   "Content-Disposition: "
#define WEB_SERVER_HEADER_CONNECTION            "Connection: "
#define WEB_SERVER_HEADER_ETAG                  "Etag: "
#define WEB_SERVER_HEADER_DATE                  "Date: "
#define WEB_SERVER_HEADER_HOST                  "Host: "
#define WEB_SERVER_HEADER_ACCEPT_ENCODING       "Accept-Encoding: "
#define WEB_SERVER_HEADER_ACCEPT_LANGUAGE       "Accept-Language: "
#define WEB_SERVER_HEADER_CONTENT_LANGUAGE      "Content-Language: "

/* Content types list */
#define TEXT_HTML                    "text/html"
#define TEXT_CSS                     "text/css"
#define TEXT_XML                     "text/xml"
#define APPLICATION_JSON             "application/json"
#define IMAGE_PNG                    "image/png"
#define IMAGE_GIF                    "image/gif"
#define TEXT_PLAIN                   "text/plain"
#define TEXT_CSV                     "text/csv"
#define APPLICATION_JAVASCRIPT       "application/javascript"
#define IMAGE_JPEG                   "image/jpeg"
#define APPLICATION_PDF              "application/pdf"
#define APPLICATION_ZIP              "application/zip"
#define SHOCKWAVE_FLASH              "application/x-shockwave-flash"
#define AUDIO_X_AAC                  "audio/x-aac"
#define IMAGE_X_ICON                 "image/x-icon"
#define TEXT_VCARD                   "text/vcard"
#define APPLICATION_OCTEC_STREAM     "application/octet-stream"
#define VIDEO_AVI                    "video/avi"
#define VIDEO_MPEG                   "video/mpeg"
#define VIDEO_MP4                    "video/mp4"
#define FORM_URLENCODED              "application/x-www-form-urlencoded"

/* MIME types list */
#define TEXT_HTML_MIME                  ".html"
#define TEXT_CSS_MIME                   ".css"
#define TEXT_XML_MIME                   ".xml"
#define APPLICATION_JSON_MIME           ".json"
#define IMAGE_PNG_MIME                  ".png"
#define IMAGE_GIF_MIME                  ".gif"
#define TEXT_PLAIN_MIME                 ".txt"
#define TEXT_CSV_MIME                   ".csv"
#define APPLICATION_JAVASCRIPT_MIME     ".js"
#define IMAGE_JPEG_MIME                 ".jpg"
#define APPLICATION_PDF_MIME            ".pdf"
#define APPLICATION_ZIP_MIME            ".zip"
#define SHOCKWAVE_FLASH_MIME            ".swf"
#define AUDIO_X_AAC_MIME                ".aac"
#define IMAGE_X_ICON_MIME               ".ico"
#define TEXT_VCARD_MIME                 ".vcf"
#define APPLICATION_OCTEC_STREAM_MIME   ".bin"
#define VIDEO_AVI_MIME                  ".avi"
#define VIDEO_MPEG_MIME                 ".mpeg"
#define VIDEO_MP4_MIME                  ".mp4"
/* dummy - no such extension */

#define URL_ENCODED_MIME                ".form"    
typedef enum
{
/* Content types list */
    HttpContentTypeList_TextHtml,
    HttpContentTypeList_TextCSS,
    HttpContentTypeList_TextXML,
    HttpContentTypeList_ApplicationJson,
    HttpContentTypeList_ImagePNG,
    HttpContentTypeList_ImageGIF,
    HttpContentTypeList_TextPlain,
    HttpContentTypeList_TextCSV,
    HttpContentTypeList_ApplicationJavascript,
    HttpContentTypeList_ImageJPEG,
    HttpContentTypeList_ApplicationPDF,
    HttpContentTypeList_ApplicationZIP,
    HttpContentTypeList_ShokewaveFlash,
    HttpContentTypeList_AudioXAAC,
    HttpContentTypeList_ImageXIcon,
    HttpContentTypeList_TextVcard,
    HttpContentTypeList_ApplicationOctecStream,
    HttpContentTypeList_VideoAVI,
    HttpContentTypeList_VideoMPEG,
    HttpContentTypeList_VideoMP4,
    HttpContentTypeList_UrlEncoded,
}HttpContentTypeList;

#define CONTENT_LEN_TYPE    0xFF

/* offsets of TLV structure of parameters parsed in NetApp request */
#define ARGV_TYPE_OFFSET     0
#define ARGV_LEN_OFFSET      2
#define ARGV_VALUE_OFFSET    3

typedef struct    _http_headerFieldType_t_
{
    SlNetAppMetadataHTTPTypes_e headerType;
    char * headerText;
}http_headerFieldType_t;

typedef struct    _http_contentTypeMapping_t_
{
    HttpContentTypeList contentType;
    char *contentTypeText;
    char *mimeExt;
}http_contentTypeMapping_t;

typedef struct    _http_charValuesPair_t_
{
    char    *characteristic;
    char    *value[5];
}http_charValuesPair_t;

typedef struct    _http_RequestObj_t_
{
    uint8_t requestIdx;
    uint8_t httpMethod;
    char                      *service;
    http_charValuesPair_t charValues[10];
    int32_t (*serviceCallback)(uint8_t,
                               uint8_t *,
                               uint8_t **,
                               SlNetAppRequest_t *);
}http_RequestObj_t;

typedef enum
{
    LedIdx_RedLed,
    LedIdx_OrangeLed,
    LedIdx_GreenLed,
    LedIdx_MaxLed,
}LedIdx;

typedef enum
{
    LedValues_Off,
    LedValues_On,
    LedValues_Toggle,
    LedValues_MaxLed,
}LedValues;

typedef enum
{
    SensorIdx_XAxis,
    SensorIdx_YAxis,
    SensorIdx_ZAxis,
    SensorIdx_FarnTemp,
    SensorIdx_MaxSensor,
}SensorIdx;

typedef enum
{
    DeviceIdx_Ssid,
    DeviceIdx_IpAddress,
    DeviceIdx_MacAddress,
    DeviceIdx_AppID,
    DeviceIdx_MaxDevice,
}DeviceIdx;

typedef enum
{
    OtaIdx_Version,
    OtaIdx_MaxOTA,
}OtaIdx;

typedef struct LinkLocal_ControlBlock_t
{
    sem_t otaReportServerStartSignal;
    sem_t otaReportServerStopSignal;
    mqd_t reportServerMQueue;
}LinkLocal_CB;

/****************************************************************************
                      GLOBAL VARIABLES
****************************************************************************/
extern LinkLocal_CB LinkLocal_ControlBlock;

//****************************************************************************
//                      FUNCTION PROTOTYPES
//****************************************************************************

//*****************************************************************************
//
//! \brief This function fetches the device type and prints it
//!
//! \param[in]  None
//!
//! \return device type
//!
//****************************************************************************
uint32_t getDeviceType();

//*****************************************************************************
//
//! \brief This task handles LinkLocal transactions with the client
//!
//! \param[in]  None
//!
//! \return None
//!
//****************************************************************************
void * linkLocalTask(void *pvParameters);

#endif //_SIMPLELINK_CC32XX_LINKLOCAL_TASK_H_
