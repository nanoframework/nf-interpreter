//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Texas Instruments Incorporated.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


/* standard includes */
#include <stdlib.h>
#include <string.h>

/* TI-DRIVERS Header files */
#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>

/* Example/Board Header files */
#include <targetSimpleLinkCC32xx_Threads.h>
#include <targetSimpleLinkCC32xx_LinkLocalTask.h>
#include <targetSimpleLinkCC32xx_ProvisioningTask.h>

/* driverlib Header files */
#include <ti/devices/cc32xx/inc/hw_memmap.h>
#include <ti/devices/cc32xx/inc/hw_gprcm.h>

// nanoFramework header files
#include <targetHAL.h>

#define NETAPP_MAX_RX_FRAGMENT_LEN     SL_NETAPP_REQUEST_MAX_DATA_LEN
#define NETAPP_MAX_METADATA_LEN        (100)
#define NETAPP_MAX_ARGV_TO_CALLBACK    SL_FS_MAX_FILE_NAME_LENGTH + 50
#define NUMBER_OF_URI_SERVICES         (6)

#define LED_TOGGLE_OTA_PROCESS_TIMEOUT (100)   /* In msecs */

#define DEV_TYPE_CC3220R               (0x010)
#define DEV_TYPE_CC3220RS              (0x018)
#define DEV_TYPE_CC3220FS              (0x019)
#define DEV_TYPE_CC323XR               (0x110)
#define DEV_TYPE_CC323XRS              (0x118)
#define DEV_TYPE_CC323XFS              (0x119)

#define MAC_ADDR_STR_LEN               (18)
#define IP_ADDR_STR_LEN                (16)
#define SENSOR_VALUE_STR_LEN           (4)


/****************************************************************************
                      LOCAL FUNCTION PROTOTYPES
****************************************************************************/

//*****************************************************************************
//
//! \brief This function prepare error netapp response in case memory could
//!  not be allocated
//!
//! \param[in]  pNetAppResponse    netapp response structure
//!
//! \return none
//!
//****************************************************************************
void NetAppRequestErrorResponse(SlNetAppResponse_t *pNetAppResponse);

//*****************************************************************************
//
//! \brief This function fetches the device MAC address
//!
//! \param[out]  buffer to occupy the MAC address in string format
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t getDeviceMacAddress(uint8_t *macAddress);

//*****************************************************************************
//
//! Function to read accelarometer
//!
//! \param  none
//!
//! \return SUCCESS or FAILURE
//!
//*****************************************************************************
uint8_t accelarometerReading(void);

//*****************************************************************************
//
//! Function to read temperature
//!
//! \param  none
//!
//! \return SUCCESS or FAILURE
//!
//*****************************************************************************
uint8_t temperatureReading(void);

//*****************************************************************************
//
//! \brief this function composes an element type from metadata/payload 
//!(TLV structure)
//!
//! \param[in]  isAnswer           states whether this is a value or a parameter
//!
//! \param[in]  requestIdx         request index to indicate the message
//!
//! \param[in]  elementVal         value of element
//!
//! \return element type
//!
//****************************************************************************
uint16_t setElementType(uint8_t isValue,
                        uint8_t requestIdx,
                        uint8_t elementVal);

//*****************************************************************************
//
//! \brief This function prepares metadata for HTTP GET requests
//!
//! \param[in] parsingStatus        validity of HTTP GET request
//!
//! \param[in] contentLen           content length in respond to  
//! HTTP GET request
//!
//! \return metadataLen
//!
//****************************************************************************
uint16_t prepareGetMetadata(int32_t parsingStatus,
                            uint32_t contentLen,
                            HttpContentTypeList contentTypeId);

//*****************************************************************************
//
//! \brief This function prepares metadata for HTTP POST/PUT requests
//!
//! \param[in] parsingStatus        validity of HTTP POST/PUT request
//!
//! \return metadataLen
//!
//****************************************************************************
uint16_t preparePostMetadata(int32_t parsingStatus);

//*****************************************************************************
//
//! \brief This function fetches the device IP address
//!
//! \param[in]  none
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t getDeviceIpAddress(void);

//*****************************************************************************
//
//! \brief This function fetches the SSID the device is connected to
//!
//! \param[in]  none
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t getDeviceSSID(void);

//*****************************************************************************
//
//! \brief This function flushes the netapp data from the client
//!
//! \param[in] netAppRequest        netapp request structure
//!
//! \param[in] flags                netapp flags for more data
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t otaFlushNetappReq(SlNetAppRequest_t *netAppRequest,
                          uint32_t *flags);

//*****************************************************************************
//
//! \brief This is a filesystem service callback function for HTTP PUT
//!
//! \param[in]  requestIdx        request index to indicate the message
//!
//! \param[in]  argcCallback      count of input params to the service callback
//!
//! \param[in]  argvCallback      set of input params to the service callback
//!
//! \param[in] netAppRequest      netapp request structure
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t otaPutCallback(uint8_t requestIdx,
                       uint8_t *argcCallback,
                       uint8_t **argvCallback,
                       SlNetAppRequest_t *netAppRequest);

//*****************************************************************************
//
//! \brief This is the ota service callback function for HTTP GET
//!
//! \param[in]  requestIdx        request index to indicate the message
//!
//! \param[in]  argcCallback      count of input params to the service callback
//!
//! \param[in]  argvCallback      set of input params to the service callback
//!
//! \param[in] netAppRequest      netapp request structure
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t otaGetCallback(uint8_t requestIdx,
                       uint8_t *argcCallback,
                       uint8_t **argvCallback,
                       SlNetAppRequest_t *netAppRequest);

//*****************************************************************************
//
//! \brief This is a light service callback function for HTTP GET
//!
//! \param[in]  requestIdx        request index to indicate the message
//!
//! \param[in]  argcCallback      count of input params to the service callback
//!
//! \param[in]  argvCallback      set of input params to the service callback
//!
//! \param[in] netAppRequest      netapp request structure
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t lightGetCallback(uint8_t requestIdx,
                         uint8_t *argcCallback,
                         uint8_t **argvCallback,
                         SlNetAppRequest_t *netAppRequest);

//*****************************************************************************
//
//! \brief This is a light service callback function for HTTP POST
//!
//! \param[in]  requestIdx        request index to indicate the message
//!
//! \param[in]  argcCallback      count of input params to the service callback
//!
//! \param[in]  argvCallback      set of input params to the service callback
//!
//! \param[in] netAppRequest      netapp request structure
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t lightPostCallback(uint8_t requestIdx,
                          uint8_t *argcCallback,
                          uint8_t **argvCallback,
                          SlNetAppRequest_t *netAppRequest);

//*****************************************************************************
//
//! \brief This is a sensors service callback function for HTTP GET
//!
//! \param[in]  requestIdx        request index to indicate the message
//!
//! \param[in]  argcCallback      count of input params to the service callback
//!
//! \param[in]  argvCallback      set of input params to the service callback
//!
//! \param[in] netAppRequest      netapp request structure
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t sensorGetCallback(uint8_t requestIdx,
                          uint8_t *argcCallback,
                          uint8_t **argvCallback,
                          SlNetAppRequest_t *netAppRequest);

//*****************************************************************************
//
//! \brief This is a generic device service callback function for HTTP GET
//!
//! \param[in]  requestIdx        request index to indicate the message
//!
//! \param[in]  argcCallback      count of input params to the service callback
//!
//! \param[in]  argvCallback      set of input params to the service callback
//!
//! \param[in] netAppRequest      netapp request structure
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t deviceGetCallback(uint8_t requestIdx,
                          uint8_t *argcCallback,
                          uint8_t **argvCallback,
                          SlNetAppRequest_t *netAppRequest);

//*****************************************************************************
//
//! \brief This function create mailbox message queue between linkLocal task
//!                and driver task
//!
//! \param[in]  None
//!
//! \return 0 on success or -ve on error
//!
//****************************************************************************
void initLinkLocalDB(void);

//*****************************************************************************
//
//! \brief This function scan netapp request and parse the payload
//!
//! \param[in]  requestIdx        request index to indicate the message
//!
//! \param[in]  pPhrase           pointer to HTTP metadata payload
//!
//! \param[in]  payloadLen        HTTP metadata or payload length
//!
//! \param[out]  argcCallback     count of input params to the service callback
//!
//! \param[out]  argvCallback     set of input params to the service callback
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t parseUrlEncoded(uint8_t requestIdx,
                        uint8_t * pPhrase,
                        uint16_t phraseLen,
                        uint8_t *argcCallback,
                        uint8_t **argvCallback);

//*****************************************************************************
//
//! \brief This function maps header type to its string value
//!
//! \param[in]  httpHeaderType        http header type
//!
//! \param[out]  httpHeaderText       http header text
//!
//! \return none
//!
//****************************************************************************
void convertHeaderType2Text(uint8_t httpHeaderType,
                            uint8_t **httpHeaderText);

//*****************************************************************************
//
//! \brief This function scan netapp request and parse the metadata
//!
//! \param[in]  requestType       HTTP method (GET, POST, PUT or DEL)
//!
//! \param[in]  pMetadata         pointer to HTTP metadata
//!
//! \param[in]  metadataLen       HTTP metadata length
//!
//! \param[out]  requestIdx       request index to indicate the message
//!
//! \param[out]  argcCallback     count of input params to the service callback
//!
//! \param[out]  argvCallback     set of input params to the service callback
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t parseHttpRequestMetadata(uint8_t requestType,
                                 uint8_t * pMetadata,
                                 uint16_t metadataLen,
                                 uint8_t *requestIdx,
                                 uint8_t *argcCallback,
                                 uint8_t **argvCallback);

//*****************************************************************************
//
//! \brief This function scan netapp request and parse the payload
//!
//! \param[in]  requestIdx        request index to indicate the message
//!
//! \param[in]  pPayload          pointer to HTTP payload
//!
//! \param[in]  payloadLen        HTTP payload length
//!
//! \param[out]  argcCallback     count of input params to the service callback
//!
//! \param[out]  argvCallback     set of input params to the service callback
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t parseHttpRequestPayload(uint8_t requestIdx,
                                uint8_t * pPayload,
                                uint16_t payloadLen,
                                uint8_t *argcCallback,
                                uint8_t **argvCallback);

//*****************************************************************************
//
//! \brief This function checks that the content requested via HTTP message exists
//!
//! \param[in] netAppRequest      netapp request structure
//!
//! \param[out]  requestIdx       request index to indicate the message
//!
//! \param[out]  argcCallback     count of input params to the service callback
//!
//! \param[out]  argvCallback     set of input params to the service callback
//!
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t httpCheckContentInDB(SlNetAppRequest_t *netAppRequest,
                             uint8_t *requestIdx,
                             uint8_t *argcCallback,
                             uint8_t **argvCallback);

//*****************************************************************************
//
//! \brief This function parse and execute HTTP GET requests
//!
//! \param[in] netAppRequest        netapp request structure
//!
//! \return None
//!
//****************************************************************************
void httpGetHandler(SlNetAppRequest_t *netAppRequest);

//*****************************************************************************
//
//! \brief This function parse and execute HTTP POST/PUT requests
//!
//! \param[in] netAppRequest        netapp request structure
//!
//! \return None
//!
//****************************************************************************
void httpPostHandler(SlNetAppRequest_t *netAppRequest);

/****************************************************************************
                      GLOBAL VARIABLES
****************************************************************************/

const uint8_t pageNotFound[] = "<html>404 - Sorry page not found</html>";

/* metadata and content buffer are allocated 
   static as these are shared in all use cases*/
/* however, it is possible to set those dynamically as well */
uint8_t gMetadataBuffer[NETAPP_MAX_METADATA_LEN];
uint8_t gPayloadBuffer[NETAPP_MAX_RX_FRAGMENT_LEN];
//uint8_t gOtaVersion[VERSION_STR_SIZE + 1];

uint8_t gHttpPostBuffer[NETAPP_MAX_ARGV_TO_CALLBACK];
uint8_t gHttpGetBuffer[NETAPP_MAX_ARGV_TO_CALLBACK];

int8_t xVal, yVal, zVal;
float temperatureVal;
I2C_Handle i2cHandle;

/* database to hold ota archive */
//OtaArchive_t gOtaArcive;

/* message queue for http messages between server and client */
mqd_t linkLocalMQueue;
pthread_mutex_t *sensorLockObj = NULL;    /* Lock Object for sensor readings */

http_RequestObj_t httpRequest[NUMBER_OF_URI_SERVICES] =
{
    {0, SL_NETAPP_REQUEST_HTTP_GET, "/ota", {
         {NULL}
     }, NULL},
    {1, SL_NETAPP_REQUEST_HTTP_PUT, "/ota", {
         {NULL}
     }, NULL},
    {2, SL_NETAPP_REQUEST_HTTP_GET, "/light", {
         {NULL}
     }, NULL},
    {3, SL_NETAPP_REQUEST_HTTP_POST, "/light", {
         {NULL}
     }, NULL},
    {4, SL_NETAPP_REQUEST_HTTP_GET, "/sensor", {
         {NULL}
     }, NULL},
    {5, SL_NETAPP_REQUEST_HTTP_GET, "/device", {
         {NULL}
     }, NULL},
};

http_headerFieldType_t g_HeaderFields [] =
{
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_VERSION, WEB_SERVER_VERSION},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_REQUEST_URI, WEB_SERVER_REQUEST_URI},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_QUERY_STRING,
     WEB_SERVER_QUERY_STRING},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONTENT_TYPE,
     WEB_SERVER_HEADER_CONTENT_TYPE},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONTENT_LEN,
     WEB_SERVER_HEADER_CONTENT_LEN},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_LOCATION, 
     WEB_SERVER_HEADER_LOCATION},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_SERVER, WEB_SERVER_HEADER_SERVER},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_USER_AGENT,
     WEB_SERVER_HEADER_USER_AGENT},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_COOKIE, WEB_SERVER_HEADER_COOKIE},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_SET_COOKIE,
     WEB_SERVER_HEADER_SET_COOKIE},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_UPGRADE, WEB_SERVER_HEADER_UPGRADE},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_REFERER, WEB_SERVER_HEADER_REFERER},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_ACCEPT, WEB_SERVER_HEADER_ACCEPT},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONTENT_ENCODING,
     WEB_SERVER_HEADER_CONTENT_ENCODING},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONTENT_DISPOSITION,
     WEB_SERVER_HEADER_CONTENT_DISPOSITION},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONNECTION,
     WEB_SERVER_HEADER_CONNECTION},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_ETAG, WEB_SERVER_HEADER_ETAG},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_DATE, WEB_SERVER_HEADER_DATE},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HEADER_HOST, WEB_SERVER_HEADER_HOST},
    {SL_NETAPP_REQUEST_METADATA_TYPE_ACCEPT_ENCODING,
     WEB_SERVER_HEADER_ACCEPT_ENCODING},
    {SL_NETAPP_REQUEST_METADATA_TYPE_ACCEPT_LANGUAGE,
     WEB_SERVER_HEADER_ACCEPT_LANGUAGE},
    {SL_NETAPP_REQUEST_METADATA_TYPE_CONTENT_LANGUAGE,
     WEB_SERVER_HEADER_CONTENT_LANGUAGE}
};

http_contentTypeMapping_t g_ContentTypes [] =
{
    {HttpContentTypeList_TextHtml, TEXT_HTML, TEXT_HTML_MIME},
    {HttpContentTypeList_TextCSS, TEXT_CSS, TEXT_CSS_MIME},
    {HttpContentTypeList_TextXML, TEXT_XML, TEXT_XML_MIME},
    {HttpContentTypeList_ApplicationJson, APPLICATION_JSON,
     APPLICATION_JSON_MIME},
    {HttpContentTypeList_ImagePNG, IMAGE_PNG, IMAGE_PNG_MIME},
    {HttpContentTypeList_ImageGIF, IMAGE_GIF, IMAGE_GIF_MIME},
    {HttpContentTypeList_TextPlain, TEXT_PLAIN, TEXT_PLAIN_MIME},
    {HttpContentTypeList_TextCSV, TEXT_CSV, TEXT_CSV_MIME},
    {HttpContentTypeList_ApplicationJavascript, APPLICATION_JAVASCRIPT,
     APPLICATION_JAVASCRIPT_MIME},
    {HttpContentTypeList_ImageJPEG, IMAGE_JPEG, IMAGE_JPEG_MIME},
    {HttpContentTypeList_ApplicationPDF, APPLICATION_PDF, 
     APPLICATION_PDF_MIME},
    {HttpContentTypeList_ApplicationZIP, APPLICATION_ZIP, 
     APPLICATION_ZIP_MIME},
    {HttpContentTypeList_ShokewaveFlash, SHOCKWAVE_FLASH, 
     SHOCKWAVE_FLASH_MIME},
    {HttpContentTypeList_AudioXAAC, AUDIO_X_AAC, AUDIO_X_AAC_MIME},
    {HttpContentTypeList_ImageXIcon, IMAGE_X_ICON, IMAGE_X_ICON_MIME},
    {HttpContentTypeList_TextVcard, TEXT_VCARD, TEXT_VCARD_MIME},
    {HttpContentTypeList_ApplicationOctecStream, APPLICATION_OCTEC_STREAM,
     APPLICATION_OCTEC_STREAM_MIME},
    {HttpContentTypeList_VideoAVI, VIDEO_AVI, VIDEO_AVI_MIME},
    {HttpContentTypeList_VideoMPEG, VIDEO_MPEG, VIDEO_MPEG_MIME},
    {HttpContentTypeList_VideoMP4, VIDEO_MP4, VIDEO_MP4_MIME},
    {HttpContentTypeList_UrlEncoded, FORM_URLENCODED, URL_ENCODED_MIME}
};

LinkLocal_CB LinkLocal_ControlBlock;

extern int snprintf(char *_string,
                    size_t _n,
                    const char *_format,
                    ...);

/*****************************************************************************
                  Callback Functions
*****************************************************************************/

//*****************************************************************************
//
//! \brief This function is registered as netapp request callback
//!
//! \param[in]  pNetAppRequest        netapp request structure
//!
//! \param[out]  pNetAppResponse    netapp response structure
//!
//! \return none
//!
//****************************************************************************
void SimpleLinkNetAppRequestEventHandler(SlNetAppRequest_t *pNetAppRequest,
                                         SlNetAppResponse_t *pNetAppResponse)
{
    SlNetAppRequest_t *netAppRequest;
    int32_t msgqRetVal;

    INFO_PRINT(
        "[Link local task] NetApp Request Received - AppId = %d, Type = %d,"
        "Handle = %d\n\r",
        pNetAppRequest->AppId, pNetAppRequest->Type, pNetAppRequest->Handle);

    if((pNetAppRequest->Type == SL_NETAPP_REQUEST_HTTP_GET) ||
       (pNetAppRequest->Type == SL_NETAPP_REQUEST_HTTP_DELETE) ||
       (pNetAppRequest->Type == SL_NETAPP_REQUEST_HTTP_POST) ||
       (pNetAppRequest->Type == SL_NETAPP_REQUEST_HTTP_PUT))
    {
        /* Prepare pending response */
        pNetAppResponse->Status = SL_NETAPP_RESPONSE_PENDING;
        pNetAppResponse->ResponseData.pMetadata = NULL;
        pNetAppResponse->ResponseData.MetadataLen = 0;
        pNetAppResponse->ResponseData.pPayload = NULL;
        pNetAppResponse->ResponseData.PayloadLen = 0;
        pNetAppResponse->ResponseData.Flags = 0;
    }
    else
    {
        NetAppRequestErrorResponse(pNetAppResponse);

        return;
    }

    netAppRequest = (SlNetAppRequest_t *) malloc (sizeof(SlNetAppRequest_t));
    if(NULL == netAppRequest)
    {
        NetAppRequestErrorResponse(pNetAppResponse);

        return;
    }

    memset(netAppRequest,0,sizeof(SlNetAppRequest_t));
    netAppRequest->AppId = pNetAppRequest->AppId;
    netAppRequest->Type = pNetAppRequest->Type;
    netAppRequest->Handle = pNetAppRequest->Handle;
    netAppRequest->requestData.Flags = pNetAppRequest->requestData.Flags;

    /* Copy Metadata */
    if(pNetAppRequest->requestData.MetadataLen > 0)
    {
        netAppRequest->requestData.pMetadata = (uint8_t *) malloc (
            pNetAppRequest->requestData.MetadataLen);
        if(NULL == netAppRequest->requestData.pMetadata)
        {
            NetAppRequestErrorResponse(pNetAppResponse);
            free(netAppRequest);
            return;
        }
        sl_Memcpy(netAppRequest->requestData.pMetadata,
                  pNetAppRequest->requestData.pMetadata,
                  pNetAppRequest->requestData.MetadataLen);
        netAppRequest->requestData.MetadataLen =
            pNetAppRequest->requestData.MetadataLen;
    }
    else
    {
        netAppRequest->requestData.MetadataLen = 0;
    }

    /* Copy the payload */
    if(pNetAppRequest->requestData.PayloadLen > 0)
    {
        netAppRequest->requestData.pPayload = (uint8_t *) malloc (
            pNetAppRequest->requestData.PayloadLen);
        if(NULL == netAppRequest->requestData.pPayload)
        {
            NetAppRequestErrorResponse(pNetAppResponse);

            if(netAppRequest->requestData.pMetadata != NULL)
            {
                free(netAppRequest->requestData.pMetadata);
            }
            free(netAppRequest);
            return;
        }
        sl_Memcpy (netAppRequest->requestData.pPayload,
                   pNetAppRequest->requestData.pPayload,
                   pNetAppRequest->requestData.PayloadLen);
        netAppRequest->requestData.PayloadLen =
            pNetAppRequest->requestData.PayloadLen;
    }
    else
    {
        netAppRequest->requestData.PayloadLen = 0;
    }

    msgqRetVal = mq_send(linkLocalMQueue, (char *)&netAppRequest, 1, 0);
    if(msgqRetVal < 0)
    {
        UART_PRINT("[Link local task] could not send element to msg queue\n\r");
        while(1)
        {
            ;
        }
    }
}

//*****************************************************************************
//
//! \brief This is a filesystem service callback function for HTTP PUT
//!
//! \param[in]  requestIdx          request index to indicate the message
//!
//! \param[in]  argcCallback        count of input params to the 
//!                                    service callback
//!
//! \param[in]  argvCallback        set of input params to the service callback
//!
//! \param[in] netAppRequest        netapp request structure
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t otaPutCallback(uint8_t requestIdx,
                       uint8_t *argcCallback,
                       uint8_t **argvCallback,
                       SlNetAppRequest_t *netAppRequest)
{
    uint8_t *argvArray;
    uint16_t metadataLen;
    uint8_t *filename;
    int32_t Status;
    uint32_t flags;
    uint32_t fileLen = 0;
    uint16_t elementType;
    int16_t otaState, processedBytes, unprocessedBytes, chunkLen, otaChunkLen;
    int32_t accumulatedLen;
    uint8_t otaProgressBar;
    uint32_t deviceType;
    struct timespec ts;

    StartLedEvtTimer(LED_TOGGLE_OTA_PROCESS_TIMEOUT);

    Status = 0;
    accumulatedLen = 0;
    deviceType = getDeviceType();
    argvArray = *argvCallback;

    /* get the filename */
    filename = argvArray + 2 * ARGV_VALUE_OFFSET + 1;

    elementType = setElementType(1, requestIdx, CONTENT_LEN_TYPE);

    /* search for the file length */
    while((*argcCallback > 0) && (*((uint16_t *)argvArray) != elementType))
    {
        argvArray += ARGV_LEN_OFFSET;        /* skip the type */
        argvArray += *argvArray;    /* add the length */
        argvArray++;        /* skip the length */

        (*argcCallback)--;
    }

    if(*((uint16_t *)argvArray) == elementType)
    {
        sl_Memcpy ((uint8_t*)&fileLen, (argvArray + ARGV_VALUE_OFFSET),
                   *(argvArray + ARGV_LEN_OFFSET));
    }

    UART_PRINT("[Link local task] Received OTA filename %s, len = %lu \n\r",
               filename,
               fileLen);

    /* Init the Tar parser module */
    //OtaArchive_Init(&gOtaArcive);

    otaProgressBar = 0;

    /* at this point start the report server in the background */
    /* signal to ota task */
    sem_post(&LinkLocal_ControlBlock.otaReportServerStartSignal);

    /* set the flags to check for more data */
    flags = netAppRequest->requestData.Flags;

    /* must be archive file name */
    if(strstr((const char *)filename, ".tar") == NULL)
    {
        UART_PRINT(
            "[Link local task] OTA filename should be in *.tar format\n\r");
        Status = -1;
        goto exit_ota_put;
    }

    /* updating versions */
    // OtaArchive_CheckVersion(&gOtaArcive, filename);

    sl_Memcpy(gPayloadBuffer, netAppRequest->requestData.pPayload,
              netAppRequest->requestData.PayloadLen);
    // Status =
    //     (int32_t)OtaArchive_Process(&gOtaArcive, gPayloadBuffer,
    //                                 netAppRequest->requestData.PayloadLen,
    //                                 &processedBytes);
    INFO_PRINT("[Link local task] Received OTA payload %d. Processed %d \n\r",
               netAppRequest->requestData.PayloadLen,
               processedBytes);

    // if(Status < 0)
    // {
    //     goto exit_ota_put;
    // }
    // else if(Status == ARCHIVE_STATUS_DOWNLOAD_DONE)
    // {
    //     otaProgressBar = 100;
    //     mq_send(LinkLocal_ControlBlock.reportServerMQueue,
    //             (char *)&otaProgressBar, 1,
    //             0);

    //     /* Tar file parsing completed  */
    //     //UART_PRINT(
    //         // "[Link local task] sl_extLib_OtaRun: ---- "
    //         // "Download file completed %s\r\n",
    //         // filename);

    //     Status = 0;

    //     goto exit_ota_put;
    // }
    unprocessedBytes = netAppRequest->requestData.PayloadLen - processedBytes;
    accumulatedLen += processedBytes;
    otaProgressBar = (accumulatedLen * 100) / fileLen;
    mq_send(LinkLocal_ControlBlock.reportServerMQueue, (char *)&otaProgressBar,
            1,
            0);

    while(OOB_IS_NETAPP_MORE_DATA(flags) ||
          ((unprocessedBytes > 0) &&
           ((fileLen - accumulatedLen) < NETAPP_MAX_RX_FRAGMENT_LEN)))
    {
        /* copy the unprocessed part to the start of the buffer */
        if(unprocessedBytes > 0)
        {
            sl_Memcpy(&gPayloadBuffer[0], &gPayloadBuffer[processedBytes],
                      unprocessedBytes);
        }

        if(OOB_IS_NETAPP_MORE_DATA(flags))
        {
            chunkLen = NETAPP_MAX_RX_FRAGMENT_LEN - unprocessedBytes;
            Status =
                sl_NetAppRecv(netAppRequest->Handle, (uint16_t *)&chunkLen,
                              &gPayloadBuffer[unprocessedBytes],
                              (unsigned long *)&flags);
            INFO_PRINT(
                "[Link local task] sl_NetAppRecv payload=%d, flags=%d \n\r",
                chunkLen, flags);
            if(Status < 0)
            {
                UART_PRINT(
                    "[Link local task] sl_NetAppRecv error=%d, flags=%d \n\r",
                    Status, flags);
                /* Stop the parsing of the archive file */
               // OtaArchive_Abort(&gOtaArcive);

                goto exit_ota_put;
            }
        }
        else
        {
            /* cover cases where netapp has no more data and
               download is close to 100%.
               Archive module needs more processing time to complete */
            INFO_PRINT(
                "[Link local task] No more data in NetApp but archive module "
				"still has processing to do \n\r");
            Status = 0;
            chunkLen = 0;
        }

        //otaState = OtaArchive_GetSktatus(&gOtaArcive);
        //INFO_PRINT("[Link local task] OTA state is %d \n\r", otaState);
        // if(otaState == OtaArchiveState_OpenFile)
        // {
        //     //INFO_PRINT("[Link local task] File size is %d \n\r",
        //             //    gOtaArcive.CurrTarObj.FileSize);
        // }

        // if(otaState == OtaArchiveState_OpenFile)
        // {
        //     if(strstr((const char *)gOtaArcive.CurrTarObj.pFileName,
        //               "mcuimg.bin") != NULL)
        //     {
        //         if((deviceType == DEV_TYPE_CC3220FS) ||
        //            (deviceType == DEV_TYPE_CC323XFS))
        //         {
        //             //UART_PRINT(
        //                 // "[Link local task] mcu image of CC32xxR or CC32xxRS "
        //                 // "cannot be programmed onto CC32xxSF\n\r");

        //             /* Stop the parsing of the archive file */
        //             OtaArchive_Abort(&gOtaArcive);

        //             goto exit_ota_put;
        //         }
        //     }

        //     if(strstr((const char *)gOtaArcive.CurrTarObj.pFileName,
        //               "mcuflashimg.bin") != NULL)
        //     {
        //         if((deviceType == DEV_TYPE_CC3220R)  ||
        //            (deviceType == DEV_TYPE_CC3220RS) ||
        //            (deviceType == DEV_TYPE_CC323XR)  ||
        //            (deviceType == DEV_TYPE_CC323XRS))
        //         {
        //             //UART_PRINT(
        //                 // "[Link local task] mcu image of CC32xxSF cannot be "
		// 				// "programmed onto CC32xxR or CC32xxRS\n\r");

        //             /* Stop the parsing of the archive file */
        //             OtaArchive_Abort(&gOtaArcive);

        //             goto exit_ota_put;
        //         }
        //     }
        // }

        otaChunkLen = chunkLen + unprocessedBytes;
        // Status =
        //     (int32_t)OtaArchive_Process(&gOtaArcive, gPayloadBuffer,
        //                                 otaChunkLen,
        //                                 &processedBytes);

        INFO_PRINT(
            "[Link local task] Received OTA payload=%d. Processed=%d \n\r",
            otaChunkLen, processedBytes);
        if(Status < 0)
        {
            UART_PRINT("[Link local task] OtaArchive error %d \n\r", Status);
            goto exit_ota_put;
        }
        // else if(Status == ARCHIVE_STATUS_DOWNLOAD_DONE)
        // {
        //     clock_gettime(CLOCK_REALTIME, &ts);
        //     ts.tv_nsec += 1000000;
        //     if(ts.tv_nsec > 1000000000)
        //     {
        //         ts.tv_nsec -= 1000000000;
        //         ts.tv_sec++;
        //     }

        //     otaProgressBar = 100;
        //     mq_timedsend(LinkLocal_ControlBlock.reportServerMQueue,
        //                  (char *)&otaProgressBar, 1, 0,
        //                  &ts);
        //     /* Tar file parsing completed  */
        //     //UART_PRINT(
        //         // "[Link local task] sl_extLib_OtaRun: ---- Download "
        //         // "file completed %s\r\n",
        //         // filename);

        //     Status = 0;

        //     goto exit_ota_put;
        // }
        unprocessedBytes = otaChunkLen - processedBytes;
        accumulatedLen += processedBytes;
        otaProgressBar = (accumulatedLen * 100) / fileLen;

        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_nsec += 1000000;
        if(ts.tv_nsec > 1000000000)
        {
            ts.tv_nsec -= 1000000000;
            ts.tv_sec++;
        }

        mq_timedsend(LinkLocal_ControlBlock.reportServerMQueue,
                     (char *)&otaProgressBar, 1, 0,
                     &ts);
    }

exit_ota_put:
   
    
    
    /* several use cases can occur at this point 

    
    ▼use case▼|  good  |  bad  |  no rx bufer |  internal error |
    ---------------------------------------------------------------
    Status      |   0    |  <0   |      0       |       0         |
    ---------------------------------------------------------------
    progress bar|  100   | <100  |     <100     |      <100       |
    ---------------------------------------------------------------
    flags       |   0    |  N/A  |      0       |    80000000     |
    ---------------------------------------------------------------
    send metada?|  yes   |  yes  |      yes     |       no        |
    ---------------------------------------------------------------
    notes       |        |       |   bad case   |    bad case     |
    ---------------------------------------------------------------

    
    */
    
    /* progress bar is not yet 100% - bad case*/
    if(otaProgressBar != 100)
    {
        Status = -1;
    }
    UART_PRINT("[Link local task] ota put done. Status=%d \r\n", Status);
    if(Status == 0)
    {
        /* flush the netapp data from client */
        otaFlushNetappReq(netAppRequest, &flags);
    }

    /* sending metadata is not allowed in case of internal error */
    if(OOB_IS_NETAPP_ERROR(flags))
    {
        UART_PRINT("[Link local task] sl_NetAppRecv error, flags=%d \n\r",
                   flags);
    }
    else
    {
        metadataLen = preparePostMetadata(Status);

        INFO_PRINT("[Link local task] ota put, sending metadata \r\n");
        sl_NetAppSend (netAppRequest->Handle, metadataLen, gMetadataBuffer,
                       SL_NETAPP_REQUEST_RESPONSE_FLAGS_METADATA);
    }

    if(Status != 0)
    {
        /* mark progress bar to 0xFF so ota task
           would get restarted to reopen socket */
        otaProgressBar = 0xFF;

        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_nsec += 1000000;
        if(ts.tv_nsec > 1000000000)
        {
            ts.tv_nsec -= 1000000000;
            ts.tv_sec++;
        }

        mq_timedsend(LinkLocal_ControlBlock.reportServerMQueue,
                     (char *)&otaProgressBar, 1, 0,
                     &ts);
    }

    UART_PRINT("[Link local task] waiting for signal from report server\r\n");

    /* waits for ota response server to
      finish sending report messages to client */
    sem_wait(&LinkLocal_ControlBlock.otaReportServerStopSignal);

    StopLedEvtTimer();
    if(Status == 0)
    {
        //GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);
    }
    else
    {
        //GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
    }

    if(Status == 0)
    {
        /* it means Tar file is downloaded and parsed correctly.
            Need to reset the MCU */
        mcuReboot();
    }

    return(Status);
}

//*****************************************************************************
//
//! \brief This is the ota service callback function for HTTP GET
//!
//! \param[in]  requestIdx        request index to indicate the message
//!
//! \param[in]  argcCallback      count of input params to the service callback
//!
//! \param[in]  argvCallback      set of input params to the service callback
//!
//! \param[in] netAppRequest      netapp request structure
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t otaGetCallback(uint8_t requestIdx,
                       uint8_t *argcCallback,
                       uint8_t **argvCallback,
                       SlNetAppRequest_t *netAppRequest)
{
    uint8_t *argvArray, *pPayload;
    uint16_t metadataLen, elementType;
    int32_t Status = 0;

    argvArray = *argvCallback;
    pPayload = gPayloadBuffer;

    while(*argcCallback > 0)
    {
        elementType = setElementType(1, requestIdx, CONTENT_LEN_TYPE);
        /* content length is irrelevant for GET */
        if(*((uint16_t *)argvArray) != elementType)          
        {
            switch(*(argvArray + ARGV_VALUE_OFFSET))
            {
            case OtaIdx_Version:

                // Status = OtaArchive_GetCurrentVersion(gOtaVersion);
                // if(Status < 0)
                // {
                //     //UART_PRINT(
                //         // "[Link local task] ota bundle version file does "
                //         // "not exist\r\n");
                //     strcpy((char *)gMetadataBuffer, "no version file exists");
                //     Status = 0;
                // }
                // else
                // {
                //     //sl_Memcpy(gMetadataBuffer, gOtaVersion, VERSION_STR_SIZE);
                //     gMetadataBuffer[VERSION_STR_SIZE] = '\0';
                // }

                break;
            }

            sl_Memcpy (
                pPayload,
                httpRequest[requestIdx].charValues[*(argvArray +
                                                     ARGV_VALUE_OFFSET)].
                characteristic,
                strlen((const char *)httpRequest[requestIdx].
                charValues[*(argvArray+ARGV_VALUE_OFFSET)]
                .characteristic));
                pPayload += strlen(
                (const char *)httpRequest[requestIdx].
                charValues[*(argvArray +ARGV_VALUE_OFFSET)].
                characteristic);
            *pPayload++ = '=';
            sl_Memcpy (pPayload, gMetadataBuffer,
                       strlen((const char *)gMetadataBuffer));
            pPayload += strlen((const char *)gMetadataBuffer);
            *pPayload++ = '&';
        }

        (*argcCallback)--;
        argvArray += ARGV_LEN_OFFSET;        /* skip the type */
        argvArray += *argvArray;    /* add the length */
        argvArray++;                /* skip the length */
    }

    /* NULL terminate the payload */
    *(pPayload - 1) = '\0';

    metadataLen =
        prepareGetMetadata(Status, strlen(
                               (const char *)gPayloadBuffer),
                           HttpContentTypeList_UrlEncoded);

    sl_NetAppSend (netAppRequest->Handle, metadataLen, gMetadataBuffer,
                   (SL_NETAPP_REQUEST_RESPONSE_FLAGS_CONTINUATION |
                    SL_NETAPP_REQUEST_RESPONSE_FLAGS_METADATA));
    INFO_PRINT("[Link local task] Metadata Sent, len = %d \n\r", metadataLen);
    /* mark as last segment */
    sl_NetAppSend(netAppRequest->Handle, 
                  strlen ((const char *)gPayloadBuffer),
                  gPayloadBuffer,
                   0);                            
    INFO_PRINT("[Link local task] Data Sent, len = %d\n\r",
               strlen ((const char *)gPayloadBuffer));

    return(Status);
}

//*****************************************************************************
//
//! \brief This is a light service callback function for HTTP GET
//!
//! \param[in]  requestIdx        request index to indicate the message
//!
//! \param[in]  argcCallback      count of input params to the service callback
//!
//! \param[in]  argvCallback      set of input params to the service callback
//!
//! \param[in] netAppRequest      netapp request structure
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t lightGetCallback(uint8_t requestIdx,
                         uint8_t *argcCallback,
                         uint8_t **argvCallback,
                         SlNetAppRequest_t *netAppRequest)
{
    uint8_t *argvArray, *pPayload;
    //uint8_t ledIdx = Board_GPIO_LED0;
    uint16_t ledState, metadataLen, elementType;

    argvArray = *argvCallback;
    pPayload = gPayloadBuffer;

    while(*argcCallback > 0)
    {
        elementType = setElementType(1, requestIdx, CONTENT_LEN_TYPE);
        //  /* content length is irrelevant for GET */
        // if(*((uint16_t *)argvArray) != elementType)         
        // {
        //     switch(*(argvArray + ARGV_VALUE_OFFSET))
        //     {
        //     case LedIdx_RedLed:
        //         ledIdx = Board_GPIO_LED0;
        //         break;
        //     case LedIdx_OrangeLed:
        //         ledIdx = Board_GPIO_LED0;
        //         break;
        //     case LedIdx_GreenLed:
        //         ledIdx = Board_GPIO_LED0;
        //         break;
        //     }

        //     ledState = GPIO_read(ledIdx);
        //     sl_Memcpy (
        //         pPayload,
        //         httpRequest[requestIdx].charValues[*(argvArray +
        //                                              ARGV_VALUE_OFFSET)].
        //         characteristic,
        //         strlen((const char *)httpRequest[requestIdx].
        //         charValues[*(argvArray+ARGV_VALUE_OFFSET)]
        //         .characteristic));
        //         pPayload += strlen(
        //         (const char *)httpRequest[requestIdx].
        //         charValues[*(argvArray + ARGV_VALUE_OFFSET)]
        //         .characteristic);
        //     *pPayload++ = '=';
        //     sl_Memcpy (
        //         pPayload,
        //         httpRequest[requestIdx].
        //         charValues[*(argvArray +ARGV_VALUE_OFFSET)].
        //         value[ledState],strlen((const char *)httpRequest[requestIdx].
        //                                charValues[*(argvArray +
        //                                             ARGV_VALUE_OFFSET)].
        //         value[ledState]));
                
        //     pPayload += strlen(
        //         (const char *)httpRequest[requestIdx].
        //         charValues[*(argvArray +ARGV_VALUE_OFFSET)].
        //         value[ledState]);
        //     *pPayload++ = '&';
        // }

        (*argcCallback)--;
        argvArray += ARGV_LEN_OFFSET; /* skip the type */
        argvArray += *argvArray;      /* add the length */
        argvArray++;                  /* skip the length */
    }

    /* NULL terminate the payload */
    *(pPayload - 1) = '\0';

    metadataLen = prepareGetMetadata(0,
                                    strlen((const char *)gPayloadBuffer),
                                    HttpContentTypeList_UrlEncoded);
    sl_NetAppSend (netAppRequest->Handle, metadataLen, gMetadataBuffer,
                   (SL_NETAPP_REQUEST_RESPONSE_FLAGS_CONTINUATION |
                    SL_NETAPP_REQUEST_RESPONSE_FLAGS_METADATA));
    INFO_PRINT("[Link local task] Metadata Sent, len = %d \n\r", metadataLen);
           /* mark as last segment */
    sl_NetAppSend ( netAppRequest->Handle,
                    strlen ((const char *)gPayloadBuffer),
                    gPayloadBuffer, 0);                 
    INFO_PRINT("[Link local task] Data Sent, len = %d\n\r",
               strlen ((const char *)gPayloadBuffer));

    return(0);
}

//*****************************************************************************
//
//! \brief This is a light service callback function for HTTP POST
//!
//! \param[in]  requestIdx        request index to indicate the message
//!
//! \param[in]  argcCallback      count of input params to the service callback
//!
//! \param[in]  argvCallback      set of input params to the service callback
//!
//! \param[in] netAppRequest      netapp request structure
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t lightPostCallback(uint8_t requestIdx,
                          uint8_t *argcCallback,
                          uint8_t **argvCallback,
                          SlNetAppRequest_t *netAppRequest)
{
    uint8_t *argvArray;
    uint16_t metadataLen, elementType;
    uint16_t ledState = 0xFF;
    //uint16_t ledIdx = Board_GPIO_LED0;
    argvArray = *argvCallback;

    while(*argcCallback > 0)
    {
        elementType = setElementType(1, requestIdx, CONTENT_LEN_TYPE);
        // /* content length is irrelevant for POST */
        // if(*((uint16_t *)argvArray) != elementType)          
        // {
        //     /* means it is the value, not the parameter */
        //     if(*(argvArray + 1) & 0x80)     
        //     {
        //         /* get the light operation    */
        //         switch(*(argvArray + ARGV_VALUE_OFFSET))
        //         {
        //         case LedValues_Off:
        //             ledState = Board_GPIO_LED_OFF;
        //             break;
        //         case LedValues_On:
        //             ledState = Board_GPIO_LED_ON;
        //             break;
        //         case LedValues_Toggle:
        //             ledState = 0xFF;
        //             break;
        //         }

        //         if(ledState == 0xFF)
        //         {
        //             GPIO_toggle(ledIdx);
        //         }
        //         else
        //         {
        //             GPIO_write(ledIdx, ledState);
        //         }
        //     }
        //     else    /* means it is the parameter, not the value */
        //     {
        //         /*    apply to the right light    */
        //         switch(*(argvArray + ARGV_VALUE_OFFSET))
        //         {
        //         case LedIdx_RedLed:
        //             ledIdx = Board_GPIO_LED0;
        //             break;
        //         }
        //     }
        // }

        (*argcCallback)--;
        argvArray += ARGV_LEN_OFFSET;        /* skip the type */
        argvArray += *argvArray;    /* add the length */
        argvArray++;        /* skip the length */
    }

    metadataLen = preparePostMetadata(0);

    sl_NetAppSend (netAppRequest->Handle, metadataLen, gMetadataBuffer,
                   SL_NETAPP_REQUEST_RESPONSE_FLAGS_METADATA);

    return(0);
}

//*****************************************************************************
//
//! \brief This is a sensors service callback function for HTTP GET
//!
//! \param[in]  requestIdx          request index to indicate the message
//!
//! \param[in]  argcCallback      count of input params to the service callback
//!
//! \param[in]  argvCallback      set of input params to the service callback
//!
//! \param[in] netAppRequest      netapp request structure
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t sensorGetCallback(uint8_t requestIdx,
                          uint8_t *argcCallback,
                          uint8_t **argvCallback,
                          SlNetAppRequest_t *netAppRequest)
{
    uint8_t *argvArray, *pPayload;
    uint16_t metadataLen, elementType;
    int16_t value = 0;
    int32_t Status;

    argvArray = *argvCallback;
    pPayload = gPayloadBuffer;

    /* Read accelerometer axis values */
    Status = accelarometerReading();
    if(Status != 0)
    {
        UART_PRINT(
            "[Link local task] Failed to read data from accelerometer\n\r");
    }

    /* Read temperature sensor values */
    Status = temperatureReading();
    if(Status != 0)
    {
        UART_PRINT(
            "[Link local task] Failed to"
            " read data from temperature sensor\n\r");
    }

    while(*argcCallback > 0)
    {
        elementType = setElementType(1, requestIdx, CONTENT_LEN_TYPE);
        /* content length is irrelevant for GET */
        if(*((uint16_t *)argvArray) != elementType)          
        {
            switch(*(argvArray + ARGV_VALUE_OFFSET))
            {
            case SensorIdx_XAxis:
                value = xVal;
                break;
            case SensorIdx_YAxis:
                value = yVal;
                break;
            case SensorIdx_ZAxis:
                value = zVal;
                break;
            case SensorIdx_FarnTemp:
                value = (int16_t)temperatureVal;
                break;
            }

            sl_Memcpy (
                pPayload,
                httpRequest[requestIdx].charValues[*(argvArray +
                                                     ARGV_VALUE_OFFSET)].
                characteristic,
                strlen((const char *)httpRequest[requestIdx].
                charValues[*(argvArray+ARGV_VALUE_OFFSET)].
                characteristic));
            pPayload += strlen(
                (const char *)httpRequest[requestIdx].
                charValues[*(argvArray +ARGV_VALUE_OFFSET)].
                characteristic);
            *pPayload++ = '=';

            snprintf((char *)pPayload, SENSOR_VALUE_STR_LEN, "%d", value);
            /* add the value length */
            pPayload += strlen((const char *)pPayload);    
            *pPayload++ = '&';
        }

        (*argcCallback)--;
        argvArray += ARGV_LEN_OFFSET; /* skip the type */
        argvArray += *argvArray;      /* add the length */
        argvArray++;                  /* skip the length */
    }

    /* NULL terminate the payload */
    *(pPayload - 1) = '\0';

    metadataLen = prepareGetMetadata(0,
                                    strlen((const char *)gPayloadBuffer),
                                    HttpContentTypeList_UrlEncoded);

    sl_NetAppSend (netAppRequest->Handle, metadataLen, gMetadataBuffer,
                   (SL_NETAPP_REQUEST_RESPONSE_FLAGS_CONTINUATION |
                    SL_NETAPP_REQUEST_RESPONSE_FLAGS_METADATA));
    INFO_PRINT("[Link local task] Metadata Sent, len = %d \n\r", metadataLen);
 /* mark as last segment */
    sl_NetAppSend (netAppRequest->Handle, strlen (
                       (const char *)gPayloadBuffer), gPayloadBuffer, 0);
    INFO_PRINT("[Link local task] Data Sent, len = %d\n\r",
               strlen ((const char *)gPayloadBuffer));

    return(0);
}

//*****************************************************************************
//
//! \brief This is a generic device service callback function for HTTP GET
//!
//! \param[in]  requestIdx          request index to indicate the message
//!
//! \param[in]  argcCallback        count of input params to the service callback
//!
//! \param[in]  argvCallback        set of input params to the service callback
//!
//! \param[in] netAppRequest        netapp request structure
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t deviceGetCallback(uint8_t requestIdx,
                          uint8_t *argcCallback,
                          uint8_t **argvCallback,
                          SlNetAppRequest_t *netAppRequest)
{
    uint8_t *argvArray, *pPayload;
    uint16_t metadataLen, elementType;
    int32_t Status;
    uint32_t deviceType;

    argvArray = *argvCallback;
    pPayload = gPayloadBuffer;
    deviceType = getDeviceType();

    while(*argcCallback > 0)
    {
        elementType = setElementType(1, requestIdx, CONTENT_LEN_TYPE);
        /* content length is irrelevant for GET */
        if(*((uint16_t *)argvArray) != elementType)          
        {
            switch(*(argvArray + ARGV_VALUE_OFFSET))
            {
            case DeviceIdx_Ssid:
                Status = getDeviceSSID();
                if(Status != 0)
                {
                    // UART_PRINT(
                    //     "[Link local task] failed to get SSID.  IP_ACQ=%d, "
                    //     "IP_LEASED=%d\n\r",
                    //     GET_STATUS_BIT(OutOfBox_ControlBlock.Status,
                    //                    AppStatusBits_IpAcquired),
                    //     GET_STATUS_BIT(OutOfBox_ControlBlock.Status,
                    //                    AppStatusBits_IpLeased));
                    goto exit_device_get;
                }
                break;
            case DeviceIdx_IpAddress:
                Status = getDeviceIpAddress();
                if(Status != 0)
                {
                    goto exit_device_get;
                }
                break;
            case DeviceIdx_MacAddress:
                Status = getDeviceMacAddress(gMetadataBuffer);
                if(Status != 0)
                {
                    goto exit_device_get;
                }
                break;
            case DeviceIdx_AppID:
                /* 3235 applies for both CC3230 and CC3235 */
                if(deviceType == DEV_TYPE_CC323XFS)
                {
                    strcpy((char *)gMetadataBuffer, "out_of_box_3235_fs");
                }
                else if(deviceType == DEV_TYPE_CC323XRS)
                {
                    strcpy((char *)gMetadataBuffer, "out_of_box_3235_rs");
                }
                else if(deviceType == DEV_TYPE_CC323XR)
                {
                    strcpy((char *)gMetadataBuffer, "out_of_box_3235_r");
                }
                else if(deviceType == DEV_TYPE_CC3220FS)
                {
                    strcpy((char *)gMetadataBuffer, "out_of_box_fs");
                }
                else if(deviceType == DEV_TYPE_CC3220RS)
                {
                    strcpy((char *)gMetadataBuffer, "out_of_box_rs");
                }
                else if(deviceType == DEV_TYPE_CC3220R)
                {
                    strcpy((char *)gMetadataBuffer, "out_of_box_r");
                }
                else
                {
                    UART_PRINT(
                        "[Link local task] device type %d is not supported\n\r",
                        deviceType);
                    Status = -1;

                    goto exit_device_get;
                }

                Status = 0;

                break;
            }

            sl_Memcpy (
                pPayload,
                httpRequest[requestIdx].charValues[*(argvArray +
                                                     ARGV_VALUE_OFFSET)].
                characteristic,
                strlen((const char *)httpRequest[requestIdx].
                charValues[*(argvArray+ARGV_VALUE_OFFSET)].
                characteristic));
            pPayload += strlen(
                (const char *)httpRequest[requestIdx].
                charValues[*(argvArray +ARGV_VALUE_OFFSET)].
                characteristic);
            *pPayload++ = '=';
            sl_Memcpy (pPayload, gMetadataBuffer,
                       strlen((const char *)gMetadataBuffer));
            pPayload += strlen((const char *)gMetadataBuffer);
            *pPayload++ = '&';
        }

        (*argcCallback)--;
        argvArray += ARGV_LEN_OFFSET;      /* skip the type */
        argvArray += *argvArray;           /* add the length */
        argvArray++;                       /* skip the length */
    }

    /* NULL terminate the payload */
    *(pPayload - 1) = '\0';

    Status = 0;

exit_device_get:
    if(Status != 0)
    {
        strcpy((char *)gPayloadBuffer, (const char *)pageNotFound);
    }

    metadataLen =
        prepareGetMetadata(Status,
        strlen((const char *)gPayloadBuffer),
        HttpContentTypeList_UrlEncoded);

    sl_NetAppSend (netAppRequest->Handle, metadataLen, gMetadataBuffer,
                   (SL_NETAPP_REQUEST_RESPONSE_FLAGS_CONTINUATION |
                    SL_NETAPP_REQUEST_RESPONSE_FLAGS_METADATA));
    INFO_PRINT("[Link local task] Metadata Sent, len = %d \n\r", metadataLen);
    /* mark as last segment */    
    sl_NetAppSend (netAppRequest->Handle, strlen (
                       (const char *)gPayloadBuffer), gPayloadBuffer, 0);
    INFO_PRINT("[Link local task] Data Sent, len = %d\n\r",
               strlen ((const char *)gPayloadBuffer));

    return(Status);
}

//*****************************************************************************
//                 Local Functions
//*****************************************************************************

//*****************************************************************************
//
//! \brief This function prepare error netapp response in case memory could 
//!        not be allocated
//!
//! \param[in]  pNetAppResponse    netapp response structure
//!
//! \return none
//!
//****************************************************************************
void NetAppRequestErrorResponse(SlNetAppResponse_t *pNetAppResponse)
{
    UART_PRINT(
        "[Link local task] could not allocate memory for netapp request\n\r");

    /* Prepare error response */
    pNetAppResponse->Status = SL_NETAPP_RESPONSE_NONE;
    pNetAppResponse->ResponseData.pMetadata = NULL;
    pNetAppResponse->ResponseData.MetadataLen = 0;
    pNetAppResponse->ResponseData.pPayload = NULL;
    pNetAppResponse->ResponseData.PayloadLen = 0;
    pNetAppResponse->ResponseData.Flags = 0;
}

//*****************************************************************************
//
//! \brief This function fetches the device MAC address
//!
//! \param[out]  buffer to occupy the MAC address in string format
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t getDeviceMacAddress(uint8_t *macAddress)
{
    uint8_t macAddressVal[6];
    uint16_t macAddressLen;
    int32_t Status;

    macAddress[0] = '\0';

    /* Get the device's MAC address */
    macAddressLen = 6;
    Status =
        sl_NetCfgGet(SL_NETCFG_MAC_ADDRESS_GET,NULL,&macAddressLen,
                     (uint8_t *)macAddressVal);
    if(Status < 0)
    {
        return(Status);
    }

    snprintf((char *)macAddress, MAC_ADDR_STR_LEN,
             "%02x:%02x:%02x:%02x:%02x:%02x",
             macAddressVal[0],
             macAddressVal[1],
             macAddressVal[2],
             macAddressVal[3],
             macAddressVal[4],
             macAddressVal[5]);

    return(0);
}

//*****************************************************************************
//
//! Function to read accelarometer
//!
//! \param  none
//!
//! \return SUCCESS or FAILURE
//!
//*****************************************************************************
uint8_t accelarometerReading(void)
{
    int8_t xValRead, yValRead, zValRead;
    int32_t Status;

    if(sensorLockObj != NULL)
    {
        pthread_mutex_lock(sensorLockObj);
    }

    /* Read accelarometer axis values */
    Status = BMA2xxReadNew(i2cHandle, &xValRead, &yValRead, &zValRead);
    if(Status != 0)
    {
        /* try to read again */
        Status = BMA2xxReadNew(i2cHandle, &xValRead, &yValRead, &zValRead);
        if(Status != 0)     /* leave previous values */
        {
            UART_PRINT(
                "[Link local task] Failed to read data from accelarometer\n\r");
        }
    }

    if(Status == 0)
    {
        xVal = xValRead;
        yVal = yValRead;
        zVal = zValRead;
    }

    if(sensorLockObj != NULL)
    {
        pthread_mutex_unlock(sensorLockObj);
    }

    return(Status);
}

//*****************************************************************************
//
//! Function to read temperature
//!
//! \param  none
//!
//! \return SUCCESS or FAILURE
//!
//*****************************************************************************
uint8_t temperatureReading(void)
{
    int32_t Status;
    float fTempRead;

    /* Read temperature axis values */
    Status = TMP006DrvGetTemp(i2cHandle, &fTempRead);
    if(Status != 0)
    {
        /* try to read again */
        Status = TMP006DrvGetTemp(i2cHandle, &fTempRead);
        if(Status != 0)     /* leave previous values */
        {
            UART_PRINT(
                "[Link local task] Failed to read data from"
                " temperature sensor\n\r");
        }
    }

    if(Status == 0)
    {
        fTempRead = (fTempRead > 100) ? 100 : fTempRead;
        temperatureVal = fTempRead;
    }

    return(Status);
}

//*****************************************************************************
//
//! \brief this function composes an element type from metadata/payload
//!        (TLV structure)
//!
//! \param[in]  isAnswer          states whether this is a value or a parameter
//!
//! \param[in]  requestIdx        request index to indicate the message
//!
//! \param[in]  elementVal        value of element
//!
//! \return element type
//!
//****************************************************************************
uint16_t setElementType(uint8_t isValue,
                        uint8_t requestIdx,
                        uint8_t elementVal)
{
    uint16_t elementType;

    elementType = elementVal;
    elementType |= (((isValue << 7) | (requestIdx & 0x7F)) << 8);

    return(elementType);
}

//*****************************************************************************
//
//! \brief This function prepares metadata for HTTP GET requests
//!
//! \param[in] parsingStatus        validity of HTTP GET request
//!
//! \param[in] contentLen           content length in respond to 
//!            HTTP GET request
//!
//! \return metadataLen
//!
//****************************************************************************
uint16_t prepareGetMetadata(int32_t parsingStatus,
                            uint32_t contentLen,
                            HttpContentTypeList contentTypeId)
{
    char *contentType;
    uint8_t *pMetadata;
    uint16_t metadataLen;

    contentType = g_ContentTypes[contentTypeId].contentTypeText;

    pMetadata = gMetadataBuffer;

    /* http Status */
    *pMetadata = (uint8_t) SL_NETAPP_REQUEST_METADATA_TYPE_STATUS;
    pMetadata++;
    *(uint16_t *)pMetadata = (uint16_t) 2;
    pMetadata += 2;

    if(parsingStatus < 0)
    {
        *(uint16_t *)pMetadata =
            (uint16_t) SL_NETAPP_HTTP_RESPONSE_404_NOT_FOUND;
    }
    else
    {
        *(uint16_t *)pMetadata = (uint16_t) SL_NETAPP_HTTP_RESPONSE_200_OK;
    }

    pMetadata += 2;

    /* Content type */
    *pMetadata = (uint8_t) SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONTENT_TYPE;
    pMetadata++;
    (*(uint16_t *)pMetadata) = (uint16_t) strlen ((const char *)contentType);
    pMetadata += 2;
    sl_Memcpy (pMetadata, contentType, strlen((const char *)contentType));
    pMetadata += strlen((const char *)contentType);

    /* Content len */
    *pMetadata = SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONTENT_LEN;
    pMetadata++;
    *(uint16_t *)pMetadata = (uint16_t) 4;
    pMetadata += 2;
    *(uint32_t *)pMetadata = (uint32_t) contentLen;

    metadataLen = 5 + 7 + strlen ((const char *)contentType) + 3;

    return(metadataLen);
}

//*****************************************************************************
//
//! \brief This function prepares metadata for HTTP POST/PUT requests
//!
//! \param[in] parsingStatus        validity of HTTP POST/PUT request
//!
//! \return metadataLen
//!
//****************************************************************************
uint16_t preparePostMetadata(int32_t parsingStatus)
{
    uint8_t *pMetadata;
    uint16_t metadataLen;

    pMetadata = gMetadataBuffer;

    /* http Status */
    *pMetadata = (uint8_t) SL_NETAPP_REQUEST_METADATA_TYPE_STATUS;
    pMetadata++;
    *(uint16_t *)pMetadata = (uint16_t) 2;
    pMetadata += 2;

    if(parsingStatus < 0)
    {
        *(uint16_t *)pMetadata =
            (uint16_t) SL_NETAPP_HTTP_RESPONSE_404_NOT_FOUND;
    }
    else
    {/* no need for content so browser stays on the same page */
        *(uint16_t *)pMetadata =
            (uint16_t) SL_NETAPP_HTTP_RESPONSE_204_OK_NO_CONTENT;
    }

    pMetadata += 2;

    metadataLen = 5;

    return(metadataLen);
}

//*****************************************************************************
//
//! \brief This function fetches the device IP address
//!
//! \param[in]  none
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t getDeviceIpAddress(void)
{
    uint16_t ConfigOpt;
    uint16_t ipLen;
    SlNetCfgIpV4Args_t ipV4 = {0};
    int32_t Status;

    gMetadataBuffer[0] = '\0';

    /* Get the device's IP address */
    ipLen = sizeof(SlNetCfgIpV4Args_t);
    ConfigOpt = 0;
    Status =
        sl_NetCfgGet(SL_NETCFG_IPV4_STA_ADDR_MODE,&ConfigOpt,&ipLen,
                     (uint8_t *)&ipV4);
    if(Status < 0)
    {
        return(Status);
    }

    snprintf((char *)gMetadataBuffer, IP_ADDR_STR_LEN, "%d.%d.%d.%d",
             (int)SL_IPV4_BYTE(ipV4.Ip,3),
             (int)SL_IPV4_BYTE(ipV4.Ip,2),
             (int)SL_IPV4_BYTE(ipV4.Ip,1),
             (int)SL_IPV4_BYTE(ipV4.Ip,0));

    return(0);
}

//*****************************************************************************
//
//! \brief This function fetches the SSID the device is connected to
//!
//! \param[in]  none
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t getDeviceSSID(void)
{
    uint16_t len = 32;
    uint16_t config_opt = SL_WLAN_AP_OPT_SSID;
    /* simplelink as station connected to AP */
    if(GET_STATUS_BIT(nF_ControlBlock.Status,
                      AppStatusBits_IpAcquired) &&
       GET_STATUS_BIT(nF_ControlBlock.Status, AppStatusBits_Connection))
    {
        // sl_Memcpy ((uint8_t *)gMetadataBuffer,
        //            (const uint8_t *)nF_ControlBlock.connectionSSID,
        //            nF_ControlBlock.ssidLen);
        // gMetadataBuffer[nF_ControlBlock.ssidLen] = '\0';
    }
    /* simplelink as AP with connected client */
    else if(GET_STATUS_BIT(nF_ControlBlock.Status,
                           AppStatusBits_IpAcquired) &&
            GET_STATUS_BIT(nF_ControlBlock.Status, AppStatusBits_IpLeased))                                                                                
    {
        // sl_WlanGet(SL_WLAN_CFG_AP_ID, &config_opt, &len,
        //            (uint8_t *)nF_ControlBlock.connectionSSID);
        // nF_ControlBlock.ssidLen = len - 1;
        // sl_Memcpy ((uint8_t *)gMetadataBuffer,
        //            (const uint8_t *)nF_ControlBlock.connectionSSID,
        //            nF_ControlBlock.ssidLen);
        // gMetadataBuffer[nF_ControlBlock.ssidLen] = '\0';
    }
    else
    {
        return(-1);
    }

    return(0);
}

//*****************************************************************************
//
//! \brief This function flushes the netapp data from the client
//!
//! \param[in] netAppRequest        netapp request structure
//!
//! \param[in] flags                netapp flags for more data
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t otaFlushNetappReq(SlNetAppRequest_t *netAppRequest,
                          uint32_t *flags)
{
    int32_t Status;
    int32_t chunkLen;

    Status = 0;

    while((*flags & SL_NETAPP_REQUEST_RESPONSE_FLAGS_CONTINUATION) ==
          SL_NETAPP_REQUEST_RESPONSE_FLAGS_CONTINUATION)
    {
        chunkLen = NETAPP_MAX_RX_FRAGMENT_LEN;
        Status =
            sl_NetAppRecv(netAppRequest->Handle, (uint16_t *)&chunkLen,
                          gPayloadBuffer,
                          (_u32 *)flags);
        INFO_PRINT("[Link local task] flushing NetApp packet, len=%d \n\r",
                   chunkLen);
        if(Status < 0)
        {
            return(Status);
        }

        if(*flags == 0)
        {
            break;
        }
    }

    return(Status);
}

//*****************************************************************************
//
//! \brief This function create mailbox message queue between linkLocal task
//!                and driver task
//!
//! \param[in]  None
//!
//! \return 0 on success or -ve on error
//!
//****************************************************************************
void initLinkLocalDB(void)
{
    httpRequest[0].charValues[0].characteristic = "version";
    httpRequest[0].serviceCallback = otaGetCallback;

    httpRequest[1].charValues[0].characteristic = "filename";
    httpRequest[1].serviceCallback = otaPutCallback;

    httpRequest[2].charValues[0].characteristic = "redled";
    httpRequest[2].charValues[0].value[0] = "off";
    httpRequest[2].charValues[0].value[1] = "on";
    httpRequest[2].charValues[0].value[2] = "toggle";
    httpRequest[2].charValues[1].characteristic = "orangeled";
    httpRequest[2].charValues[1].value[0] = "off";
    httpRequest[2].charValues[1].value[1] = "on";
    httpRequest[2].charValues[1].value[2] = "toggle";
    httpRequest[2].charValues[2].characteristic = "greenled";
    httpRequest[2].charValues[2].value[0] = "off";
    httpRequest[2].charValues[2].value[1] = "on";
    httpRequest[2].charValues[2].value[2] = "toggle";
    httpRequest[2].serviceCallback = lightGetCallback;

    httpRequest[3].charValues[0].characteristic = "redled";
    httpRequest[3].charValues[0].value[0] = "off";
    httpRequest[3].charValues[0].value[1] = "on";
    httpRequest[3].charValues[0].value[2] = "toggle";
    httpRequest[3].serviceCallback = lightPostCallback;

    httpRequest[4].charValues[0].characteristic = "axisx";
    httpRequest[4].charValues[1].characteristic = "axisy";
    httpRequest[4].charValues[2].characteristic = "axisz";
    httpRequest[4].charValues[3].characteristic = "temp";
    httpRequest[4].serviceCallback = sensorGetCallback;

    httpRequest[5].charValues[0].characteristic = "ssid";
    httpRequest[5].charValues[1].characteristic = "ipaddress";
    httpRequest[5].charValues[2].characteristic = "macaddress";
    httpRequest[5].charValues[3].characteristic = "appname";
    httpRequest[5].serviceCallback = deviceGetCallback;
}

//*****************************************************************************
//
//! \brief This function scan netapp request and parse the payload
//!
//! \param[in]  requestIdx        request index to indicate the message
//!
//! \param[in]  pPhrase           pointer to HTTP metadata payload
//!
//! \param[in]  payloadLen        HTTP metadata or payload length
//!
//! \param[out]  argcCallback     count of input params to the service callback
//!
//! \param[out]  argvCallback     set of input params to the service callback
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t parseUrlEncoded(uint8_t requestIdx,
                        uint8_t * pPhrase,
                        uint16_t phraseLen,
                        uint8_t *argcCallback,
                        uint8_t **argvCallback)
{
    uint8_t *token;
    uint8_t characteristic, value, isValueExpected, loopIdx;
    int32_t Status = -1;
    uint8_t *argvArray;
    uint8_t remainingLen, actualLen;
    uint16_t elementType;

    argvArray = *argvCallback;
    /* it means parameters already exist - fast forward to the end of argv */
    if(*argcCallback > 0)         
    {
        loopIdx = *argcCallback;
        while(loopIdx > 0)
        {
            argvArray += ARGV_LEN_OFFSET;        /* skip the type */
            argvArray += *argvArray;             /* add the length */
            argvArray++;                         /* skip the length */

            loopIdx--;
        }
    }

    /* check if values are expected per characteristic   */
    /* it is a 2 steps procedure:                        */
    /*     1) check that '=' sign exists                 */
    /*     2) check that the value is not NULL           */
    /* if not, return                                    */
    isValueExpected = 0;
    token = (uint8_t *)strchr((char *)pPhrase, '=');
    if(token != NULL)
    {    /* it means no value supplied */
        if((*(token + 1) == '&') || (*(token + 1) == '\0'))        
        {
            return(-1);
        }
        else
        {
            isValueExpected = 1;
        }
    }

    /* Parse payload list */
    token = (uint8_t *)strtok((char *)pPhrase, "=&");

    if(NULL == token)         /* it means there is no url encoded data */
    {
        return(0);
    }

    while(token && ((pPhrase + phraseLen) > token))
    {
        Status = -1;
        characteristic = 0;

        /* run over all possible characteristics, if exist */
        while(httpRequest[requestIdx].charValues[characteristic].characteristic
              != NULL)
        {
            if(!strncmp((const char *)token,
                        (const char *)httpRequest[requestIdx].charValues[
                            characteristic].
                        characteristic,
                        strlen((const char *)httpRequest[requestIdx].charValues
                               [
                                   characteristic].characteristic)))
            {
                Status = 0;

                /* found a characteristic. save its index number */
                (*argcCallback)++;
                elementType = setElementType(0, requestIdx, characteristic);
                sl_Memcpy ((uint8_t*)argvArray, (uint8_t*)&elementType,
                           ARGV_LEN_OFFSET);
                argvArray += ARGV_LEN_OFFSET;
                *argvArray++ = 1;        /* length field */
                *argvArray++ = characteristic;
                /* remaining length is for cases where the last value is of 
                string type */
               remainingLen =(uint8_t)
                        (phraseLen -(uint8_t)(token -pPhrase) - 
                        strlen((const char *)token) - 1);

                UART_PRINT (
                    "[Link local task] characteristic is: %s\n\r",
                    (int8_t *)httpRequest[requestIdx].
                    charValues[characteristic].
                    characteristic);
                break;
            }
            else
            {
                characteristic++;
            }
        }
        /* it means the characteristics is not valid/known */
        if(-1 == Status)         
        {
            return(Status);
        }

        token = (uint8_t *)strtok(NULL, "=&");

        if(isValueExpected)
        {
            Status = -1;
            value = 0;

            if(token != NULL)
            {
                /* it means any value is OK */
                if(NULL ==
                   httpRequest[requestIdx].
                   charValues[characteristic].value[value])
                {
                    Status = 0;

                    /* found a string value. copy its content */
                    (*argcCallback)++;
                    elementType = setElementType(1, requestIdx, value);
                    sl_Memcpy ((uint8_t*)argvArray, (uint8_t*)&elementType,
                               ARGV_LEN_OFFSET);
                    argvArray += ARGV_LEN_OFFSET;
                    if(strlen((const char *)token) > remainingLen)
                    {
                        actualLen = remainingLen;
                    }
                    else
                    {
                        actualLen = strlen((const char *)token);
                    }

                    *argvArray++ = (actualLen + 1);
                    sl_Memcpy(argvArray, token, actualLen);
                    argvArray += actualLen;
                    *argvArray++ = '\0';

                    UART_PRINT ("[Link local task] value is: %s\n\r",
                                (int8_t *)(argvArray - actualLen - 1));
                }
                else
                {
                    /* run over all possible values, if exist */
                    while(httpRequest[requestIdx].charValues[characteristic].
                          value[value] != NULL)
                    {
                        if(!strncmp((const char *)token,
                                    (const char *)httpRequest[requestIdx].
                                    charValues[
                                        characteristic].value[value],
                                    strlen((const char *)httpRequest[requestIdx
                                           ].charValues[
                                               characteristic].value[value])))
                        {
                            Status = 0;

                            /* found a value. save its index number */
                            (*argcCallback)++;
                            elementType = setElementType(1, requestIdx, value);
                            sl_Memcpy ((uint8_t*)argvArray,
                                       (uint8_t*)&elementType,
                                       ARGV_LEN_OFFSET);
                            argvArray += ARGV_LEN_OFFSET;
                            *argvArray++ = 1;            /* length field */
                            *argvArray++ = value;

                            UART_PRINT (
                                "[Link local task] value is: %s\n\r",
                                (int8_t *)httpRequest[requestIdx].
                                charValues[
                                    characteristic].value[value]);

                            break;
                        }
                        else
                        {
                            value++;
                        }
                    }
                    /* it means the value is not valid/known */
                    if(-1 == Status)         
                    {
                        return(Status);
                    }
                }
            }
            token = (uint8_t *)strtok(NULL, (const char *)"=&");
        }
    }

    return(Status);
}

//*****************************************************************************
//
//! \brief This function maps header type to its string value
//!
//! \param[in]  httpHeaderType        http header type
//!
//! \param[out]  httpHeaderText       http header text
//!
//! \return none
//!
//****************************************************************************
void convertHeaderType2Text(uint8_t httpHeaderType,
                            uint8_t **httpHeaderText)
{
    int i;
    *httpHeaderText = NULL;

    for(i = 0; i < sizeof (g_HeaderFields) / sizeof(http_headerFieldType_t);
        i++)
    {
        if(g_HeaderFields[i].headerType == httpHeaderType)
        {
            *httpHeaderText = (uint8_t *)(g_HeaderFields[i].headerText);
            break;
        }
    }
}

//*****************************************************************************
//
//! \brief This function scan netapp request and parse the metadata
//!
//! \param[in]  requestType          HTTP method (GET, POST, PUT or DEL)
//!
//! \param[in]  pMetadata            pointer to HTTP metadata
//!
//! \param[in]  metadataLen          HTTP metadata length
//!
//! \param[out]  requestIdx          request index to indicate the message
//!
//! \param[out]  argcCallback        count of input params to 
//!                                     the service callback
//!
//! \param[out]  argvCallback        set of input params to the service callback
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t parseHttpRequestMetadata(uint8_t requestType,
                                 uint8_t * pMetadata,
                                 uint16_t metadataLen,
                                 uint8_t *requestIdx,
                                 uint8_t *argcCallback,
                                 uint8_t **argvCallback)
{
    uint8_t *pTlv;
    uint8_t *pEnd;

    int32_t Status = -1;
    uint8_t loopIdx;
    uint8_t type;
    uint16_t len;
    uint32_t value;
    uint8_t *typeText;
    uint8_t nullTerminator;
    uint8_t *argvArray;
    uint16_t elementType;

    argvArray = *argvCallback;

    *requestIdx = 0xFF;
    pTlv = pMetadata;
    pEnd = pMetadata + metadataLen;

    if(metadataLen < 3)
    {
        UART_PRINT("[Link local task] Metadata parsing error\n\r");

        return(-1);
    }

    INFO_PRINT ("[Link local task] Metadata:\n\r");

    while(pTlv < pEnd)
    {
        type = *pTlv;
        pTlv++;
        len = *(uint16_t *)pTlv;
        pTlv += 2;

        convertHeaderType2Text(type, &typeText);

        if(typeText != NULL)
        {
            INFO_PRINT ("[Link local task] %s ", typeText);
        }

        switch(type)
        {
        case SL_NETAPP_REQUEST_METADATA_TYPE_STATUS:
           /* there are browsers that seem to send many 0 type for no reason */
           /* in this case, do not print anything */
            break;

        case SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONTENT_LEN:
        /* it means there is a content length and URI is OK. Add it to the argv */
            if(0 == Status)         
            {
                /* it means parameters already exist from query type */
                if(*argcCallback > 0)             
                {
                    loopIdx = *argcCallback;
                    while(loopIdx > 0)
                    {
                        argvArray += ARGV_LEN_OFFSET;   /* skip the type */
                        argvArray += *argvArray;        /* add the length */
                        argvArray++;                    /* skip the length */

                        loopIdx--;
                    }
                }

                (*argcCallback)++;
                /* add content type */
                elementType = setElementType(1, *requestIdx, CONTENT_LEN_TYPE);        
                sl_Memcpy ((uint8_t*)argvArray, (uint8_t*)&elementType,
                           ARGV_LEN_OFFSET);
                argvArray += ARGV_LEN_OFFSET;
                *argvArray++ = len;        /* add content length */
                sl_Memcpy ((uint8_t*)argvArray, pTlv, len);
                sl_Memcpy ((uint8_t*)&value, pTlv, len);

                INFO_PRINT ("%d\n\r", (uint32_t)value);
            }

            break;

        case SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_REQUEST_URI:
        /* this is the 1st stop in every http method.
            zero out the character counter argument */
            *argcCallback = 0;        

            for(loopIdx = 0; loopIdx < NUMBER_OF_URI_SERVICES; loopIdx++)
            {
                if((strncmp((const char *)pTlv,
                            (const char *)httpRequest[loopIdx].service,
                           strlen((const char *)httpRequest[loopIdx].service)))
                   == 0)
                {
                    if(requestType == httpRequest[loopIdx].httpMethod)
                    {
                        Status = 0;
                        *requestIdx = httpRequest[loopIdx].requestIdx;
                        INFO_PRINT ("%s\n\r", httpRequest[loopIdx].service);

                        break;
                    }
                }
            }

            if(Status != 0)
            {
                INFO_PRINT ("unknown service\n\r");
            }

            break;

        case SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_QUERY_STRING:
            if(0 == Status)
            {
                Status = parseUrlEncoded(*requestIdx, pTlv, len, argcCallback,
                                         argvCallback);

                if(Status != 0)
                {
                    INFO_PRINT (
                       "query string in metadata section is not"
                       " valid/known\n\r");
                }
            }

            break;

        default:
            nullTerminator = *(pTlv + len);
            *(pTlv + len) = '\0';
            INFO_PRINT("%s\n\r", pTlv);
            *(pTlv + len) = nullTerminator;

            break;
        }
        pTlv += len;
    }

    return(Status);
}

//*****************************************************************************
//
//! \brief This function scan netapp request and parse the payload
//!
//! \param[in]  requestIdx          request index to indicate the message
//!
//! \param[in]  pPayload            pointer to HTTP payload
//!
//! \param[in]  payloadLen          HTTP payload length
//!
//! \param[out]  argcCallback       count of input params to the
//!                                    service callback
//!
//! \param[out]  argvCallback       set of input params to the service callback
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t parseHttpRequestPayload(uint8_t requestIdx,
                                uint8_t * pPayload,
                                uint16_t payloadLen,
                                uint8_t *argcCallback,
                                uint8_t **argvCallback)
{
    int32_t Status = -1;

    Status = parseUrlEncoded(requestIdx, pPayload, payloadLen, argcCallback,
                             argvCallback);

    if(Status != 0)
    {
        UART_PRINT (
            "[Link local task] query string in payload section is "
            "not valid/known\n\r");
    }

    return(Status);
}

//*****************************************************************************
//
//! \brief This function checks that the content requested via HTTP 
//!        message exists
//!
//! \param[in] netAppRequest        netapp request structure
//!
//! \param[out]  requestIdx         request index to indicate the message
//!
//! \param[out]  argcCallback       count of input params to the service
//!                                    callback
//!
//! \param[out]  argvCallback       set of input params to the service 
//!                                    callback
//!
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t httpCheckContentInDB(SlNetAppRequest_t *netAppRequest,
                             uint8_t *requestIdx,
                             uint8_t *argcCallback,
                             uint8_t **argvCallback)
{
    int32_t Status = -1;

    if(netAppRequest->AppId != SL_NETAPP_HTTP_SERVER_ID)
    {
        return(Status);
    }

    Status =
        parseHttpRequestMetadata(netAppRequest->Type,
                                 netAppRequest->requestData.pMetadata,
                                 netAppRequest->requestData.MetadataLen,
                                 requestIdx, argcCallback,
                                 argvCallback);

    /* PUT does not contain parseable data - only POST does */
    if((0 == Status) && (netAppRequest->requestData.PayloadLen != 0) &&
       (netAppRequest->Type != SL_NETAPP_REQUEST_HTTP_PUT))
    {
        Status =
            parseHttpRequestPayload(*requestIdx,
                                    netAppRequest->requestData.pPayload,
                                    netAppRequest->requestData.PayloadLen,
                                    argcCallback,
                                    argvCallback);
    }

    return(Status);
}

//*****************************************************************************
//
//! \brief This function parse and execute HTTP GET requests
//!
//! \param[in] netAppRequest        netapp request structure
//!
//! \return None
//!
//****************************************************************************
void httpGetHandler(SlNetAppRequest_t *netAppRequest)
{
    uint16_t metadataLen;
    int32_t Status;
    uint8_t requestIdx;

    uint8_t argcCallback;
    uint8_t     *argvArray;
    uint8_t     **argvCallback = &argvArray;

    argvArray = gHttpGetBuffer;

    Status = httpCheckContentInDB(netAppRequest, &requestIdx, &argcCallback,
                                  argvCallback);

    if(Status < 0)
    {
        metadataLen =
            prepareGetMetadata(Status, strlen (
                                   (const char *)pageNotFound),
                               HttpContentTypeList_TextHtml);

        sl_NetAppSend (netAppRequest->Handle, metadataLen, gMetadataBuffer,
                       (SL_NETAPP_REQUEST_RESPONSE_FLAGS_CONTINUATION |
                        SL_NETAPP_REQUEST_RESPONSE_FLAGS_METADATA));
        INFO_PRINT("[Link local task] Metadata Sent, len = %d \n\r",
                   metadataLen);
/* mark as last segment */
        sl_NetAppSend (netAppRequest->Handle,
        strlen ((const char *)pageNotFound),
        (uint8_t *)pageNotFound,0); 
        
        INFO_PRINT("[Link local task] Data Sent, len = %d\n\r",
                   strlen ((const char *)pageNotFound));
    }
    else
    {
        httpRequest[requestIdx].serviceCallback(requestIdx, &argcCallback,
                                                argvCallback,
                                                netAppRequest);
    }
}

//*****************************************************************************
//
//! \brief This function parse and execute HTTP POST/PUT requests
//!
//! \param[in] netAppRequest        netapp request structure
//!
//! \return None
//!
//****************************************************************************
void httpPostHandler(SlNetAppRequest_t *netAppRequest)
{
    uint16_t metadataLen;
    int32_t Status;
    uint8_t requestIdx;

    uint8_t argcCallback;
    uint8_t     *argvArray;
    uint8_t     **argvCallback = &argvArray;

    argvArray = gHttpPostBuffer;

    Status = httpCheckContentInDB(netAppRequest,&requestIdx,&argcCallback,
                                  argvCallback);

    if(Status < 0)
    {
        metadataLen = preparePostMetadata(Status);

        sl_NetAppSend (netAppRequest->Handle, metadataLen, gMetadataBuffer,
                       SL_NETAPP_REQUEST_RESPONSE_FLAGS_METADATA);
    }
    else
    {
        httpRequest[requestIdx].serviceCallback(requestIdx, &argcCallback,
                                                argvCallback,
                                                netAppRequest);
    }
}

//****************************************************************************
//                            MAIN FUNCTION
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
uint32_t getDeviceType(void)
{
    uint32_t deviceType;
    uint16_t configSize = 0;
    uint8_t  configOpt  = SL_DEVICE_GENERAL_VERSION;
    SlDeviceVersion_t ver = {0};
    configSize = sizeof(SlDeviceVersion_t);

    /* Print device version info. */
    sl_DeviceGet(SL_DEVICE_GENERAL, &configOpt, &configSize, (uint8_t*)(&ver));

    deviceType = ver.ChipId & 0xFF;
    /* Update deviceType to 323XX or 3220X */
    if((HWREG(GPRCM_BASE + GPRCM_O_GPRCM_DIEID_READ_REG4) >> 24) & 0x02)
    {
        deviceType |= (0x01 << 8);
    }

    switch(deviceType)
    {
        case DEV_TYPE_CC323XR:  // 323xR
            UART_PRINT("[Provisioning task] detected device is CC323xR\n\r");
            break;
        case DEV_TYPE_CC323XRS: // 323xRS
            UART_PRINT("[Provisioning task] detected device is CC323xRS\n\r");
            break;
        case DEV_TYPE_CC323XFS: // 323xFS
            UART_PRINT("[Provisioning task] detected device is CC323xSF\n\r");
            break;
        case DEV_TYPE_CC3220R:  // 3220R
            UART_PRINT("[Provisioning task] detected device is CC3220R\n\r");
            break;
        case DEV_TYPE_CC3220RS: // 3220RS
            UART_PRINT("[Provisioning task] detected device is CC3220RS\n\r");
            break;
        case DEV_TYPE_CC3220FS: // 3220FS
            UART_PRINT("[Provisioning task] detected device is CC3220SF\n\r");
            break;
        default:
            break;
    }

    return(deviceType);
}

//*****************************************************************************
//
//! \brief This task handles LinkLocal transactions with the client
//!
//! \param[in]  None
//!
//! \return None
//!
//****************************************************************************
void * linkLocalTask(void *pvParameters)
{
    mq_attr attr;
    int32_t msgqRetVal;
    I2C_Params i2cParams;

    /* initializes I2C */
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    // i2cHandle = I2C_open(Board_I2C0, &i2cParams);
    // if(i2cHandle == NULL)
    // {
    //     //UART_PRINT("[Link local task] Error Initializing I2C\n\r");
    // }

    /* Setup mutex operations for sensors reading */
    sensorLockObj = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(sensorLockObj, (pthread_mutexattr_t*)NULL);

    /* initializes mailbox for http messages */
    attr.mq_maxmsg = 10;         /* queue size */
    attr.mq_msgsize = sizeof(SlNetAppRequest_t*);        /* Size of message */
    linkLocalMQueue = mq_open("linklocal msg q", O_CREAT, 0, &attr);
    if(linkLocalMQueue == NULL)
    {
        UART_PRINT("[Link local task] could not create msg queue\n\r");
        while(1)
        {
            ;
        }
    }

    initLinkLocalDB();

    /* waits for valid local connection - via provisioning task  */
    sem_wait(&Provisioning_ControlBlock.provisioningDoneSignal);

    while(1)
    {
        SlNetAppRequest_t *netAppRequest;

        msgqRetVal =
            mq_receive(linkLocalMQueue, (char *)&netAppRequest,
                       sizeof(SlNetAppRequest_t*), NULL);
        if(msgqRetVal < 0)
        {
            UART_PRINT(
                "[Link local task] could not receive element from msg \
                queue\n\r");
            while(1)
            {
                ;
            }
        }

        INFO_PRINT(
            "[Link local task] NetApp Request Received - handle from main "
            "context AppId = %d, Type = %d, Handle = %d\n\r",
            netAppRequest->AppId, netAppRequest->Type, netAppRequest->Handle);

        INFO_PRINT("[Link local task] Metadata len = %d\n\r",
                   netAppRequest->requestData.MetadataLen);

        if((netAppRequest->Type == SL_NETAPP_REQUEST_HTTP_GET) ||
           (netAppRequest->Type == SL_NETAPP_REQUEST_HTTP_DELETE))
        {
            if(netAppRequest->Type == SL_NETAPP_REQUEST_HTTP_GET)
            {
                UART_PRINT("[Link local task] HTTP GET Request\n\r");
            }
            else
            {
                UART_PRINT("[Link local task] HTTP DELETE Request\n\r");
            }

            httpGetHandler(netAppRequest);
        }
        else if((netAppRequest->Type == SL_NETAPP_REQUEST_HTTP_POST) ||
                (netAppRequest->Type == SL_NETAPP_REQUEST_HTTP_PUT))
        {
            if(netAppRequest->Type == SL_NETAPP_REQUEST_HTTP_POST)
            {
                UART_PRINT("[Link local task] HTTP POST Request\n\r");
            }
            else
            {
                UART_PRINT("[Link local task] HTTP PUT Request\n\r");
            }

            INFO_PRINT(
                "[Link local task] Data received, len = %d, flags= %x\n\r",
                netAppRequest->requestData.PayloadLen,
                netAppRequest->requestData.Flags);

            httpPostHandler(netAppRequest);
        }

        if(netAppRequest->requestData.MetadataLen > 0)
        {
            free (netAppRequest->requestData.pMetadata);
        }
        if(netAppRequest->requestData.PayloadLen > 0)
        {
            free (netAppRequest->requestData.pPayload);
        }

        free (netAppRequest);
    }
}
