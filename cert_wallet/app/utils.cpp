/*
 * Copyright 2018 Alberto Sonnino
 * 
 * This file is part of SGX-WALLET.
 * 
 * SGX-WALLET is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * SGX-WALLET is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with SGX-WALLET.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <fstream>
//#include <uinstd.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "app.h"
#include "wallet.h"
#include "enclave.h"
using namespace std;
struct memory {
    size_t size;
    char *memory;
};
/**
 * @brief      Prints an info message. 
 *
 */
void info_print(const char* str) {
    printf("[INFO] %s\n", str);
}


/**
 * @brief      Prints a warning message.
 *
 */
void warning_print(const char* str) {
    printf("[WARNING] %s\n", str);
}


/**
 * @brief      Prints an error message. 
 *
 */
void error_print(const char* str) {
    printf("[ERROR] %s\n", str);
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size* nmemb;
    struct memory *mem = (struct memory *) userp;
    char *ptr = (char *)realloc(mem->memory,mem->size + realsize +1);
    if(ptr ==NULL){
    return 0;
    }
    mem->memory =ptr;
    memcpy(&mem->memory[mem->size],contents,realsize);
    mem->size +=realsize;
    mem->memory[mem->size]=0;
    return realsize;
}
static size_t header_callback(char *contents, size_t size,
                                size_t nmemb, void *userp)
{
    size_t realsize = size* nmemb;
    struct memory *mem = (struct memory *) userp;
    char *ptr = (char *)realloc(mem->memory,mem->size + realsize +1);
    if(ptr ==NULL){
    return 0;
    }
    mem->memory =ptr;
    memcpy(&mem->memory[mem->size],contents,realsize);
    mem->size +=realsize;
    mem->memory[mem->size]=0;
    return realsize;
}
/**
 * @brief      Prints the wallet's content.
 *
 */
void print_wallet( wallet_t* wallet, size_t wallet_size, char* ttoken) {


struct memory chunk;
chunk.memory= NULL;
chunk.size=0;
char grantarray[7];
string url11= "http://localhost/hbl/hbl.grant.php";
printf("\n %s\n",url11.c_str());
CURL *curl;
CURLcode res;
curl_global_init(CURL_GLOBAL_ALL);
curl = curl_easy_init();
if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost/hbl/hbl.grant.php");

    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);

    /* pass in custom data to the callback */
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &chunk);

    res=curl_easy_perform(curl);
    
    /* cleanup curl stuff */
    if(res!=CURLE_OK){
    printf("error perform %s\n",curl_easy_strerror(res));
    }
    else{
    char *granttoken= NULL;
    
    granttoken = strstr(chunk.memory,"x-custom-header");
    
    if(granttoken){
    //Etag: "3147526947"
    
    int index= (granttoken-chunk.memory)+17;
    for(int i=0; i<7; i++){
        if(i==6){
            grantarray[i]='\0';
        }else{
            grantarray[i] = chunk.memory[index + i];
        }
    }
    //memcpy(array,((etag-chunk.memory)+3);
    //printf("found granttoken at index:%ld\n",(granttoken-chunk.memory));
    printf("\ngrant token from hbl is %s\n", grantarray);
    }
    //printf("we got %d bytes\n call back memory is %p\n", (int)chunk.size,chunk.memory );
    //free(granttoken);
    }
curl_easy_cleanup(curl);    
}
/* we are done with libcurl, so clean it up */
free(chunk.memory); 


string url= "http://localhost/cert/sign.php?username=";
url=url + wallet->items[0].username;
url= url +"&signature=";
url=url + wallet->items[0].decrypted;
url=url + "&granttoken=";
url=url + grantarray;
printf("\n %s\n",url.c_str());


struct memory chunk2;
chunk2.memory= NULL;
chunk2.size=0;
char granttokennadra[7];
char tokennadra[7];

CURL *curl2;
CURLcode res1;
curl2 = curl_easy_init();

if(curl2) {
    curl_easy_setopt(curl2, CURLOPT_URL, url.c_str());

    curl_easy_setopt(curl2, CURLOPT_HEADERFUNCTION, header_callback);

    /* pass in custom data to the callback */
    curl_easy_setopt(curl2, CURLOPT_HEADERDATA, &chunk2);

    res=curl_easy_perform(curl2);
    
    /* cleanup curl stuff */
    if(res1!=CURLE_OK){
    printf("error perform %s\n",curl_easy_strerror(res1));
    }
    else{
        //usleep(1);
        char *granttoken=NULL;
        char *token= NULL;
    granttoken = strstr(chunk2.memory,"x-custom-header");
    token = strstr(chunk2.memory,"x-custom-token");
    //printf("\n granttoken=%s and token = %s\n",granttoken,token);
    if(token && granttoken){

    //Etag: "3147526947"
    int indextoken= (token-chunk2.memory)+16;
    int index= (granttoken-chunk2.memory)+17;
    for(int i=0; i<7; i++){
        if(i==6){
            tokennadra[i]='\0';
            granttokennadra[i]='\0';
        }else{
            tokennadra[i] = chunk2.memory[indextoken +i];
            granttokennadra[i] = chunk2.memory[index +i];
        }
    }
    //memcpy(array,((etag-chunk2.memory)+3);
    //printf("found granttoken at index:%ld and token at index: %ld\n",(granttoken-chunk2.memory),(token-chunk2.memory));
    printf("\ntoken recieved form nadra is %s and grant token send by nadra is %s\n", tokennadra,granttokennadra);
    }
    // free(granttoken);
    // free(token);
    //printf("we got %d bytes\n call back memory is %p\n", (int)chunk2.size,chunk2.memory );
    }

curl_easy_cleanup(curl2);    
}
free(chunk2.memory);

string url3= "http://localhost/hbl/hbl.verify.php?granttoken=";
url3=url3 + granttokennadra;
url3= url3 + "&token=";
url3=url3 + tokennadra;
printf("\n %s\n",url3.c_str());
struct memory chunk3;
chunk3.memory= NULL;
chunk3.size=0;
//char array3[10];
CURL *curl3;
CURLcode res3;
curl3 = curl_easy_init();
if(curl3) {
    curl_easy_setopt(curl3, CURLOPT_URL, url3.c_str());

    curl_easy_setopt(curl3, CURLOPT_HEADERFUNCTION, header_callback);

    /* pass in custom data to the callback */
    curl_easy_setopt(curl3, CURLOPT_HEADERDATA, &chunk3);

    res=curl_easy_perform(curl3);
    
    /* cleanup curl stuff */
    if(res3!=CURLE_OK){
    printf("error perform %s\n",curl_easy_strerror(res3));
    }
    else{
    printf("we got %d bytes\n call back memory is %p\n", (int)chunk3.size,chunk3.memory );
    }

curl_easy_cleanup(curl3);    
}

free(chunk3.memory);

curl_global_cleanup();
    
        for(int i=0; i<7; i++){
        if(i==6){
            ttoken[i]='\0';
        }else{
            ttoken[i] = tokennadra[i];
        }
    }
//ttoken = tokennadra;
}



void print_encr(const wallet_t* wallet,size_t wallet_size){

        ofstream file(ENC_File, ios::out | ios::binary);
    for (int i = 0; i < wallet->size; ++i) {

    file.write((char *)&wallet->items[i].encrypted, sizeof(wallet->items[i].encrypted));
    file << endl << endl;
    file.write((char *)&wallet->items[i].decrypted, sizeof(wallet->items[i].decrypted));
    file << endl << endl;
    file.write((char *)&wallet->items[i].encrypteee, sizeof(wallet->items[i].encrypteee));
    file << endl << endl;
    file.write((char *)&wallet->items[i].nadratoken, sizeof(wallet->items[i].nadratoken));
    file << endl << endl;
    }
    file.close();
    
    printf("\n-----------------------------------------\n\n");
    printf("%s v%s\n", APP_NAME, VERSION);
    printf("Simple password wallet based on Intel SGX.\n\n");
    printf("Number of items: %lu\n\n", wallet->size);
    for (int i = 0; i < wallet->size; ++i) {
        printf("#%d -- %s\n", i, wallet->items[i].title);
        printf("[username:] %s\n", wallet->items[i].username);
        printf("[certificate:] %s\n", wallet->items[i].certificate);
        printf("[encrypted :] %x\n", wallet->items[i].encrypteee);
        printf("[decrypted :] %s\n", wallet->items[i].decrypted);
        printf("[tokne :] %s\n", wallet->items[i].nadratoken);
        printf("\n");
    }
    printf("\n------------------------------------------\n\n");

}


/**
 * @brief      Prints an error message correspondig to the
 *             error code.
 *
 */
int is_error(int error_code) {
    char err_message[100];

    // check error case
    switch(error_code) {
        case RET_SUCCESS:
            return 0;

        case ERR_PASSWORD_OUT_OF_RANGE:
            sprintf(err_message, "Password should be at least 8 characters long and at most %d.", MAX_ITEM_SIZE);
            break;

        case ERR_WALLET_ALREADY_EXISTS:
            sprintf(err_message, "Wallet already exists: delete file '%s' first.", WALLET_FILE);
            break;

        case ERR_CANNOT_SAVE_WALLET:
            strcpy(err_message, "Coud not save wallet.");
            break;

        case ERR_CANNOT_LOAD_WALLET:
            strcpy(err_message, "Coud not load wallet.");
            break;

        case ERR_WRONG_MASTER_PASSWORD:
            strcpy(err_message, "Wrong master password."); 
            break;

        case ERR_WALLET_FULL:
            sprintf(err_message, "Wallet full (maximum number of item: %d).", MAX_ITEMS);
            break;

        case ERR_ITEM_DOES_NOT_EXIST: 
            strcpy(err_message, "Item does not exist."); 
            break;

        case ERR_ITEM_TOO_LONG:
            sprintf(err_message, "Item too longth (maximum size: %d).", MAX_ITEM_SIZE); 
            break;

        case ERR_FAIL_SEAL:
            sprintf(err_message, "Fail to seal wallet."); 
            break;

        case ERR_FAIL_UNSEAL:
            sprintf(err_message, "Fail to unseal wallet."); 
            break;

        default:
            sprintf(err_message, "Unknown error."); 
    }

    // print error message
    error_print(err_message);
    return 1;
}


/**
 * @brief      Prints help & usage. 
 *
 */
void show_help() {
	const char* command = "[-h Show this screen] [-v Show version] [-t Run tests] " \
		"[-n master-password] [-p master-password -c new-master-password]" \
		"[-p master-password -a -x items_title -y items_username -z toitems_password]" \
		"[-p master-password -r items_index]";
	printf("\nusage: %s %s\n\n", APP_NAME, command);
    printf("more information: https://github.com/asonnino/sgx-wallet\n\n");
}


/**
 * @brief      Prints the build version. 
 *
 */
void show_version() {
	printf("v%s\n", VERSION);
}



