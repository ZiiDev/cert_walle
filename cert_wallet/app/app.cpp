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
 changes
 Welcome
 */
#include "enclave_u.h"
#include "sgx_urts.h"

#include <cstring>
#include <fstream>
#include <getopt.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <iostream>

#include "app.h"
#include "utils.h"
//#include "ocalls.h"
#include "debug.h"
#include "wallet.h"
#include "enclave.h"
#include "test.h"
using namespace std;
using namespace std::chrono;


/***************************************************
 * OCALLs implementation
 ***************************************************/
/**
 * @brief      Prints a message to the console. 
 *
 */
void ocall_debug_print(const char* str) {
    printf("[DEBUG] %s\n", str);
}

/**
 * @brief      Save sealed data to file The sizes/length of 
 *             pointers need to be specified, otherwise SGX will
 *             assume a count of 1 for all pointers.
 *
 */
int ocall_save_wallet(const uint8_t* sealed_data, const size_t sealed_size) {
    ofstream file(WALLET_FILE, ios::out | ios::binary);
    if (file.fail()) {return 1;}
    file.write((const char*) sealed_data, sealed_size);
    file.close();
    return 0;
}
/*
int ocall_save_dec_wallet(wallet_t wallet, wallet_t wallet_size) {


    ofstream file(ENC_File, ios::in | ios::binary);
    file.write(wallet_t wallet, wallet_t wallet_size);
    file.close();
    return 0;
}
*/
/**
 * @brief      Load sealed data from file The sizes/length of 
 *             pointers need to be specified, otherwise SGX will
 *             assume a count of 1 for all pointers.
 *
 */
int ocall_load_wallet(uint8_t* sealed_data, const size_t sealed_size) {
    ifstream file(WALLET_FILE, ios::in | ios::binary);
    if (file.fail()) {return 1;}
    file.read((char*) sealed_data, sealed_size);
    file.close();
    return 0;
}

/**
 * @brief      Verifies if a wallet files exists.
 *
 */
int ocall_is_wallet(void) {
    ifstream file(WALLET_FILE, ios::in | ios::binary);
    if (file.fail()) {return 0;} // failure means no wallet found
    file.close();
    return 1;
}


/***************************************************
 * main
 ***************************************************/
int main(int argc, char** argv) {
    // declare enclave & return variables
    sgx_enclave_id_t eid = 0;
    sgx_launch_token_t token = {0};
    int updated, ret;
    sgx_status_t ecall_status, enclave_status;


    ////////////////////////////////////////////////
    // initialise enclave
    ////////////////////////////////////////////////
    enclave_status = sgx_create_enclave(ENCLAVE_FILE, SGX_DEBUG_FLAG, &token, &updated, &eid, NULL);
    if(enclave_status != SGX_SUCCESS) {
        error_print("Fail to initialize enclave."); 
        return -1;
    }
    info_print("Enclave successfully initilised.");


    ////////////////////////////////////////////////
    // read input arguments 
    ////////////////////////////////////////////////
    const char* options = "hvtn:p:c:saex:y:z:r:";
    opterr=0; // prevent 'getopt' from printing err messages
    char err_message[100];
    int opt, stop=0;
    int h_flag=0, v_flag=0, s_flag=0, a_flag=0, t_flag=0, e_flag=0;
    char * n_value=NULL, *p_value=NULL, *c_value=NULL, *x_value=NULL, *y_value=NULL, *z_value=NULL, *r_value=NULL, *e_value=NULL;
  
    // read user input
    while ((opt = getopt(argc, argv, options)) != -1) {
        switch (opt) {
            // help
            case 'h':
                h_flag = 1;
                break;

            // version
            case 'v':
                v_flag = 1;
                break;

            // run tests
            case 't':
                t_flag = 1;
                break;

            // create new wallet
            case 'n':
                n_value = optarg;
                break;

            // encrypt data
            case 'e':
                e_flag =1;
                
                break;

            // master-password
            case 'p':
                p_value = optarg;
                break;

            // change master-password
            case 'c':
                c_value = optarg;
                break;

            // show wallet
            case 's':
                s_flag = 1;
                break;

            // add item
            case 'a': // add item flag
                a_flag = 1;
                break;
            case 'x': // item's title
                x_value = optarg;
                break;
            case 'y': // item's username
                y_value = optarg;
                break;
            case 'z': // item's password
                z_value = optarg;
                e_value =optarg;
                break;

            // remove item
            case 'r':
                r_value = optarg;
                break;

            // exceptions
            case '?':
                if (optopt == 'n' || optopt == 'p' || optopt == 'c' || optopt == 'r' ||
                    optopt == 'x' || optopt == 'y' || optopt == 'z' || optopt == 'e'
                ) {
                    sprintf(err_message, "Option -%c requires an argument.", optopt);
                }
                else if (isprint(optopt)) {
                    sprintf(err_message, "Unknown option `-%c'.", optopt);
                }
                else {
                    sprintf(err_message, "Unknown option character `\\x%x'.",optopt);
                }
                stop = 1;
                error_print(err_message);
                error_print("Program exiting.");
                break;

            default:
                error_print("Unknown option.");
        }
    }


    ////////////////////////////////////////////////
    // perform actions
    ////////////////////////////////////////////////
    if (stop != 1) {
        // show help
        if (h_flag) {
            show_help();
        }

        // show version
        else if (v_flag) {
            show_version();
        }

        // run tests
        else if(t_flag) {
            info_print("Running tests...");
            if (test(eid) != 0) {error_print("One or more tests failed.");}
            else {info_print("All tests successfully passed.");}
        }

        // create new wallet
        else if(n_value!=NULL) {
            ecall_status = ecall_create_wallet(eid, &ret, n_value);
            if (ecall_status != SGX_SUCCESS || is_error(ret)) {
                error_print("Fail to create new wallet.");
            }
            else {
                info_print("Wallet successfully created.");
            }
        }

        // change master-password
        else if (p_value!=NULL && c_value!=NULL) {
            ecall_status = ecall_change_master_password(eid, &ret, p_value, c_value);
            if (ecall_status != SGX_SUCCESS || is_error(ret)) {
                error_print("Fail change master-password.");
            }
            else {
                info_print("Master-password successfully changed.");
            }
        }

        // show wallet
        else if(p_value!=NULL && s_flag) {
            wallet_t* wallet = (wallet_t*)malloc(sizeof(wallet_t));
            ecall_status = ecall_show_wallet(eid, &ret, p_value, wallet, sizeof(wallet_t));
            if (ecall_status != SGX_SUCCESS || is_error(ret)) {
                error_print("Fail to retrieve wallet.");
            }
            else {
                info_print("Wallet successfully retrieved.");
                print_encr(wallet,sizeof(wallet_t));
            }
            free(wallet);
        }

        // add item
        else if (p_value!=NULL && a_flag && x_value!=NULL && y_value!=NULL && z_value!=NULL) {
            item_t* new_item = (item_t*)malloc(sizeof(item_t));
            strcpy(new_item->title, x_value); 
            strcpy(new_item->username, y_value); 
            strcpy(new_item->certificate, z_value);
            ecall_status = ecall_add_item(eid, &ret, p_value, new_item, sizeof(item_t));
            if (ecall_status != SGX_SUCCESS || is_error(ret)) {
                error_print("Fail to add new item to wallet.");
            }
            else {
                info_print("Item successfully added to the wallet.");
            }
            free(new_item);
        }

        // encrypt data
        else if (p_value!=NULL && e_flag && x_value!=NULL && y_value!=NULL && e_value!=NULL) {
            wallet_t* wallet = (wallet_t*)malloc(sizeof(wallet_t));
            ecall_status = ecall_show_wallet(eid, &ret, p_value, wallet, sizeof(wallet_t));
            if (ecall_status != SGX_SUCCESS || is_error(ret)) {
                error_print("Fail to retrieve wallet.");
            }
            else {
                info_print("Wallet successfully retrieved.");

                
                if(wallet->items[0].nadratoken[0]=='\0'){
                auto start =high_resolution_clock::now();
                item_t* new_item1 = (item_t*)malloc(sizeof(item_t));
                strcpy(new_item1->title, x_value); 
                strcpy(new_item1->username, y_value); 
                strcpy(new_item1->certificate, e_value);
                uint32_t sizee = sizeof(e_value)/sizeof(new_item1->certificate[0])+1;

                ecall_status = ecall_encrypt_item(eid, &ret, p_value, new_item1, sizeof(item_t),sizee);
                if (ecall_status != SGX_SUCCESS || is_error(ret)) {
                    error_print("Fail to add new item to wallet.");
                }
                else {
                    info_print("Item successfully added to the wallet.");
                    //print_encr(wallet);
                }

                //decrypt item
                ecall_status = ecall_decrypt_item(eid,&ret,p_value, new_item1,sizeof(item_t),sizee);
                if (ecall_status != SGX_SUCCESS || is_error(ret)) {
                    error_print("Fail to add new item to wallet.");
                }
                else {
                    info_print("Item successfully added to the wallet.");
                    //print_encr(wallet);
                }
                //wallet_t* wallet = (wallet_t*)realloc(sizeof(wallet_t));
                ecall_status = ecall_show_wallet(eid, &ret, p_value, wallet, sizeof(wallet_t));
                if (ecall_status != SGX_SUCCESS || is_error(ret)) {
                    error_print("Fail to retrieve wallet.");
                }
                else {
                    info_print("Wallet successfully retrieved.");
                    char token_verfied[7];
                    print_wallet(wallet,sizeof(wallet_t), token_verfied);
                    strcpy(new_item1->nadratoken,token_verfied);
                    // //item_t* new_item1 = (item_t*)malloc(sizeof(item_t));
                    // for(int i=0; i<7; i++){
                    //     if(i==6){
                    //         new_item1->nadratoken[i]='\0';
                    //     }else{
                    //     new_item1->nadratoken[i]= token_verfied[i];
                    //     }
                    // }
                    printf("token\n%s\n%s\n",new_item1->nadratoken,token_verfied);
                    ecall_status = ecall_token(eid, &ret, p_value, new_item1, sizeof(item_t));
                        if (ecall_status != SGX_SUCCESS || is_error(ret)) {
                            error_print("Fail to add new item to wallet.");
                        }
                        else {
                            info_print("token successfully added to the wallet.");
                            print_encr(wallet,sizeof(wallet_t));
                        }
                        //free(new_item2);
                }
                auto stop = high_resolution_clock::now();
                auto duration= duration_cast<microseconds>(stop - start);
                cout<<"Duration for sign up is"<<duration.count()<<endl;
                free(new_item1);
            }
            else{
                string url3= "localhost/hbl/index.php?token=";
                url3=url3 + wallet->items[0].nadratoken;
                printf("string %s\n",url3.c_str());
                CURL *curl;
                CURLcode res;
                curl = curl_easy_init();
                if(curl) {
                    curl_easy_setopt(curl, CURLOPT_URL, url3.c_str());
                    
                    res = curl_easy_perform(curl);
                    
                    curl_easy_cleanup(curl);
                    printf("migth work");
                }
            }
        }
        free(wallet);

            
    }        

        // remove item
        else if (p_value!=NULL && r_value!=NULL) {
            char* p_end;
            int index = (int)strtol(r_value, &p_end, 10);
            if (r_value == p_end) {
                error_print("Option -r requires an integer argument.");
            }
            else {
                ecall_status = ecall_remove_item(eid, &ret, p_value, index);
                if (ecall_status != SGX_SUCCESS || is_error(ret)) {
                    error_print("Fail to remove item.");
                }
                else {
                    info_print("Item successfully removed from the wallet.");
                }
            }
        }

        // display help
        else {
            error_print("Wrong inputs.");
            show_help();
        }
    }
    


    ////////////////////////////////////////////////
    // destroy enclave
    ////////////////////////////////////////////////
    enclave_status = sgx_destroy_enclave(eid);
    if(enclave_status != SGX_SUCCESS) {
        error_print("Fail to destroy enclave."); 
        return -1;
    }
    info_print("Enclave successfully destroyed.");


    ////////////////////////////////////////////////
    // exit success
    ////////////////////////////////////////////////
    info_print("Program exit success.");
    return 0;
}
