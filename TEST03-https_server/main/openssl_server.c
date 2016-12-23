/* OpenSSL server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "openssl_server.h"

#include <string.h>

#include "openssl/ssl.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"

#include "nvs_flash.h"

#include "lwip/sockets.h"
#include "lwip/netdb.h"



// this tag is used for console loging
const static char *TAG = ">>>> DEMO-RK >>>>";

// An event group is a set of event bits. Individual event bits within an event group are referenced by a bit number.
static EventGroupHandle_t wifi_event_group;
const static int CONNECTED_BIT = BIT0;


/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */




/*

#define OPENSSL_DEMO_SERVER_ACK "HTTP/1.1 200 OK\r\n" \
                                "Content-Type: text/html\r\n" \
                                "Content-Length: 98\r\n\r\n" \
                                "<html>\r\n" \
                                "<head>\r\n" \
                                "<title>OpenSSL demo</title></head><body>\r\n" \
                                "OpenSSL server demo!\r\n" \
                                "</body>\r\n" \
                                "</html>\r\n"
*/

static void rk_openssl_demo_thread(void *p)
{
    int socket_server, socket_client; 
    socklen_t addr_len;
    //char addr_str[INET_ADDRSTRLEN];
    int proceed = 1;
    struct sockaddr_in server_addr, client_addr; // structure used to setup IP and port of the server

    ESP_LOGI(TAG, "Creating Server socket ......");
    // STEP1: create socket
    // socket() creates the socket and returns socket descriptor, which will be used by futher functions
    socket_server = socket(AF_INET,       // Address Family AF_INET - this is IP4
                         SOCK_STREAM,   // Type SOCK_STREAM means TCP protocol connection
                         IPPROTO_IP);  // Protocol IPPROTO_IP (0) means IP protocol
    if (socket_server == -1)
    {
        ESP_LOGI(TAG, "Could not create socket");
        proceed = 0;
    }
    else
    {
        ESP_LOGI(TAG, "DONE");
    }

    // STEP2: bind socket to particular IP and PORT
    if (proceed) 
    {
        //Prepare the server structure
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(OPENSSL_DEMO_LOCAL_TCP_PORT);

        ESP_LOGI(TAG, "Binding Server socket to IP and PORT ......");
        if( bind(socket_server,(struct sockaddr *)&server_addr , sizeof(server_addr)) < 0)
        {
            ESP_LOGI(TAG,"bind failed");
            close(socket_server);            
            proceed = 0;
        }
        else
        {
            ESP_LOGI(TAG,"DONE");
        }
    }

    // STEP3: puting the socket in listening stage 
    if (proceed)
    {
        ESP_LOGI(TAG, "Puting Server socket to listening mode ......");
        if ( listen(socket_server, 8) < 0) // 8 is the backlog, defines the maximum length to which the queue of 
                               // pending connections for socket_server may grow. If a connection request
                               // arrives when the queue is full, the client MAY receive an error with
                               // an indication of ECONNREFUSED or if the underlying protocol supports
                               // retransmission (TCP DOES), the request may be ignored so that a later reattempt
                               // at connection succeeds.
        {
            ESP_LOGI(TAG,"listen failed");
            close(socket_server);            
            proceed = 0;           
        }
        else
        {
            ESP_LOGI(TAG,"DONE");
        }        
    }

    // STEP4: wait for incomming connections from clients
    if (proceed)
    {
        ESP_LOGI(TAG, "Starting to accept requests from clients on port %d ......", ntohs(server_addr.sin_port));
        while (proceed) 
        {    
            // first arg - is the socket_server already created (socket(...)), binded (bind(...)) and listening 
            // second arg - result argument with the address of the client
            // third arg - result argument with the length of client_addr 
            if ( (socket_client = accept(socket_server, (struct sockaddr *)&client_addr, (socklen_t*)&addr_len)) )  
            {
                // inet_ntop( AF_INET, &(client_addr.sin_addr), addr_str, INET_ADDRSTRLEN );
                // getpeername( socket_client, (struct sockaddr *) &client_addr, (socklen_t*)&addr_len);
                // ESP_LOGI(TAG, "Connected with client IP=%s PORT=%d, addr_len=%d.", addr_str, ntohs(client_addr.sin_port), addr_len);
                ESP_LOGI(TAG, "Connected with client.");
                
                write(socket_client , "PONG!\r\n" , strlen("PONG!\r\n"));
                
                ESP_LOGI(TAG, "Closing connection with client");
                close(socket_client);
            }
            else
            {
                ESP_LOGI(TAG,"accept failed");
                close(socket_server);
                proceed = 0;
            } // if else
        } // end of while
    }

    // passing NULL deletes current Task
    ESP_LOGI(TAG, "Exiting server thread.");
    vTaskDelete(NULL);
    return;
}



static void openssl_demo_thread(void *p)
{

    
    int ret;
    
    //SSL_CTX *ctx;

    int socket, new_socket;
    struct sockaddr_in sock_addr;
    socklen_t addr_len;
    char recv_buf[OPENSSL_DEMO_RECV_BUF_LEN];

    /*
    SSL *ssl;

    
    
    

    
    const char send_data[] = OPENSSL_DEMO_SERVER_ACK;
    const int send_bytes = sizeof(send_data);

    extern const unsigned char cacert_pem_start[] asm("_binary_cacert_pem_start");
    extern const unsigned char cacert_pem_end[]   asm("_binary_cacert_pem_end");
    const unsigned int cacert_pem_bytes = cacert_pem_end - cacert_pem_start;

    extern const unsigned char prvtkey_pem_start[] asm("_binary_prvtkey_pem_start");
    extern const unsigned char prvtkey_pem_end[]   asm("_binary_prvtkey_pem_end");
    const unsigned int prvtkey_pem_bytes = prvtkey_pem_end - prvtkey_pem_start;   


    ESP_LOGI(TAG, "SSL server context create ......");
    //ctx = SSL_CTX_new(SSLv3_server_method());
    ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) {
        ESP_LOGI(TAG, "failed");
        goto failed1;
    }
    ESP_LOGI(TAG, "OK");

    ESP_LOGI(TAG, "SSL server context set own certification......");
    ret = SSL_CTX_use_certificate_ASN1(ctx, cacert_pem_bytes, cacert_pem_start);
    if (!ret) {
        ESP_LOGI(TAG, "failed");
        goto failed2;
    }
    ESP_LOGI(TAG, "OK");

    ESP_LOGI(TAG, "SSL server context set private key......");
    ret = SSL_CTX_use_PrivateKey_ASN1(0, ctx, prvtkey_pem_start, prvtkey_pem_bytes);
    if (!ret) {
        ESP_LOGI(TAG, "failed");
        goto failed2;
    }
    ESP_LOGI(TAG, "OK");

    */

    ESP_LOGI(TAG, "SSL server create socket ......");
    socket = socket(AF_INET, SOCK_STREAM, 0);
    if (socket < 0) {
        ESP_LOGI(TAG, "failed");
        goto failed2;
    }
    ESP_LOGI(TAG, "OK");

    ESP_LOGI(TAG, "SSL server socket bind ......");
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = 0;
    sock_addr.sin_port = htons(OPENSSL_DEMO_LOCAL_TCP_PORT);
    ret = bind(socket, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    if (ret) {
        ESP_LOGI(TAG, "failed");
        goto failed3;
    }
    ESP_LOGI(TAG, "OK");

    ESP_LOGI(TAG, "SSL server socket listen ......");
    ret = listen(socket, 32);
    if (ret) {
        ESP_LOGI(TAG, "failed");
        goto failed3;
    }
    ESP_LOGI(TAG, "OK");
    
reconnect:
    /*
    ESP_LOGI(TAG, "SSL server create ......");
    ssl = SSL_new(ctx);
    if (!ssl) {
        ESP_LOGI(TAG, "failed");
        goto failed3;
    }
    ESP_LOGI(TAG, "OK");
    */

    ESP_LOGI(TAG, "SSL server socket accept client ......");
    new_socket = accept(socket, (struct sockaddr *)&sock_addr, &addr_len);
    if (new_socket < 0) {
        ESP_LOGI(TAG, "failed" );
        goto failed4;
    }
    ESP_LOGI(TAG, "OK");

    /*
    SSL_set_fd(ssl, new_socket);

    ESP_LOGI(TAG, "SSL server accept client ......");
    ret = SSL_accept(ssl);
    if (!ret) {
        ESP_LOGI(TAG, "failed");
        goto failed5;
    }
    ESP_LOGI(TAG, "OK");
    */

    ESP_LOGI(TAG, "SSL server read message ......");
    do {

        memset(recv_buf, 0, OPENSSL_DEMO_RECV_BUF_LEN);
        //ret = SSL_read(ssl, recv_buf, OPENSSL_DEMO_RECV_BUF_LEN - 1);
        //if (ret <= 0) {
        //    break;
        //}
        //if (strstr(recv_buf, "GET / HTTP/1.1")) {
            //SSL_write(ssl, send_data, send_bytes);
        //    break;
        //}
        write(new_socket , "PONG!\r\n" , strlen("PONG!\r\n"));
        break;
    } while (1);
    
    ESP_LOGI(TAG, "result %d", ret);
    
    // SSL_shutdown(ssl);
//failed5:
    close(new_socket);
    new_socket = -1;
failed4:
    // SSL_free(ssl);
    // ssl = NULL;
    goto reconnect;
failed3:
    close(socket);
    socket = -1;
failed2:
    // SSL_CTX_free(ctx);
    // ctx = NULL;
//failed1:
    vTaskDelete(NULL);
    return ;

} 



static void rk_openssl_client_init(void)
{
    int ret;
    xTaskHandle openssl_handle;

    ret = xTaskCreate(rk_openssl_demo_thread,
                      OPENSSL_DEMO_THREAD_NAME,
                      OPENSSL_DEMO_THREAD_STACK_WORDS,
                      NULL,
                      OPENSSL_DEMO_THREAD_PRORIOTY,
                      &openssl_handle); 

    if (ret != pdPASS)  {
        ESP_LOGI(TAG, "create thread %s failed", OPENSSL_DEMO_THREAD_NAME);
    }
    else {
        ESP_LOGI(TAG, "created thread %s", OPENSSL_DEMO_THREAD_NAME);
    }
}



static esp_err_t rk_wifi_event_handler(void *ctx, system_event_t *event)
{
    // During the course of operating as a WiFi device, certain events may occur that ESP32 needs to know about.
    // see p99 for details
    switch(event->event_id) {
        // Started being a station.
        case SYSTEM_EVENT_STA_START:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START WiFi Event  - started beeing the station.");
            esp_wifi_connect();
            break;
        // we have connected to the access point.
        case SYSTEM_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_CONNECTED WiFi Event  - we are connected to access point.");
            system_event_sta_connected_t *connected = &event->event_info.connected;
            ESP_LOGI(TAG, "                           ssid     :%s", connected->ssid);
            ESP_LOGI(TAG, "                           ssid_len :%d", connected->ssid_len);
            ESP_LOGI(TAG, "                           bssid    :" MACSTR, MAC2STR(connected->bssid));
            ESP_LOGI(TAG, "                           channel  :%d", connected->channel);
            ESP_LOGI(TAG, "                           authmode :%d", connected->authmode);
            break;
        // Got an assigned IP address from the access point that we connected to while being a station.
        case SYSTEM_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP WiFi Event  - we have IP from DHCP.");
            system_event_sta_got_ip_t *got_ip = &event->event_info.got_ip;
            ESP_LOGI(TAG, "                           ip       :" IPSTR, IP2STR(&got_ip->ip_info.ip));
            ESP_LOGI(TAG, "                           mask     :" IPSTR, IP2STR(&got_ip->ip_info.netmask));
            ESP_LOGI(TAG, "                           gw       :" IPSTR, IP2STR(&got_ip->ip_info.gw));
            // now we indicate that connection has been established
            xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
            rk_openssl_client_init();
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED WiFi Event  - we were disconnected from access point.");
            system_event_sta_disconnected_t *disconnected = &event->event_info.disconnected;
            ESP_LOGI(TAG, "                           ssid     :%s", disconnected->ssid);
            ESP_LOGI(TAG, "                           ssid_len :%d", disconnected->ssid_len);
            ESP_LOGI(TAG, "                           bssid    :" MACSTR, MAC2STR(disconnected->bssid));
            ESP_LOGI(TAG, "                           reason   :%d", disconnected->reason);
            // This is a workaround as ESP32 WiFi libs don't currently
            //   auto-reassociate. 
            ESP_LOGI(TAG, "Let's reconnect again.'");
            esp_wifi_connect(); 
            // now clear CONNECTED_BIT in wifi_event_group to indicate that we are disconnected
            xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
            break;
        default:
            break;
    }
    return ESP_OK;
}



static void rk_wifi_conn_init(void)
{
    tcpip_adapter_init();
    // Create a new FreeRTOS event group.
    wifi_event_group = xEventGroupCreate();
    // Initialize the WiFi event loop processing and specify an event handler.
    ESP_ERROR_CHECK( esp_event_loop_init(rk_wifi_event_handler, NULL) );

    // Initialize WiFi configuration structure
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    
    // we need to initialize WiFi - on error halt ESP
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    
    // the configuration of WiFi will be stored in RAM only - not in flash also
    // If we save our settings in flash then it is these settings that will be used during an auto connect 
    // when we call esp_wifi_set_auto_connect() to connect at start up.
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    
    // set WiFi mode as the Station - not Access Point nor both
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );

    // set WiFi interface config 
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    
    // log data
    ESP_LOGI(TAG, "start the WIFI SSID:[%s] password:[%s]", EXAMPLE_WIFI_SSID, EXAMPLE_WIFI_PASS);
    
    // finaly start WiFi
    ESP_ERROR_CHECK( esp_wifi_start() );
}


// main function
void app_main(void)
{
    // ESP_LOHI - log information with the specific TAG
    ESP_LOGI(TAG, "Starting demo app_main function.");
    // initialising Flash Storage - in case of error the ESP will halt and the statement is written to the console
    ESP_ERROR_CHECK( nvs_flash_init() );
    rk_wifi_conn_init();
    ESP_LOGI(TAG, "Leaving the app_main function.");
}
