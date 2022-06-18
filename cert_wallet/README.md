# sgx-wallet
[![license](https://img.shields.io/badge/license-GPL3-brightgreen.svg)](https://github.com/asonnino/sgx-wallet/blob/master/LICENSE)

This is a simple password-wallet application based on Intel SGX for linux. Intel also provides a full [tutorial](https://software.intel.com/en-us/articles/introducing-the-intel-software-guard-extensions-tutorial-series) and [source code](https://github.com/IntelSoftware/Tutorial-Password-Manager-with-Intel-SGX) for Windows using Visual Studio.


## Pre-requisites
Ensure to have the Intel SGX Linux [drivers](https://github.com/intel/linux-sgx-driver) and [SDK](https://github.com/intel/linux-sgx) installed.


## Install
Install **sgx-wallet** as follows:

  - Source the Intel SGX SDK as described [here](https://github.com/intel/linux-sgx#install-the-intelr-sgx-sdk-1); if your SDK installation path is `/opt/intel/sgxsdk/`, run:
```
$ source /opt/intel/sgxsdk/environment
```

  - Clone and build the source code:
```
$ git clone https://github.com/asonnino/sgx-wallet.git
$ cd sgx-wallet
$ make
```


## Usage
**sgx-wallet** comes with a simple cli that can be run with the following options:
  - Show help:
```
sgx-wallet -h
```

  - Show version:
```
sgx-wallet -v
```

  - Run tests:
```
sgx-wallet -t
``` 

  - Create a new wallet with master-password `<master-passowrd>`:
```
sgx-wallet -n master-password
``` 

  - Change current master-password to `<new-master-password>`:
```
sgx-wallet -p master-password -c new-master-password
``` 

  - Add a new item to the wallet with title `<item_title>`, username `<item_username>`, and password `<item_password>`:
```
sgx-wallet -p master-password -a -x item_title -y item_username -z item_password
``` 

  - Remove item at index `<item_index>` from the wallet:
```
sgx-wallet -p master-password -r item_index
``` 
The wallet data are saved in a file called `wallet.seal` in the same directory as the main application. Note that you can have only one `wallet.seal` file, and attempting to call twice `sgx-wallet -n master-password` will result in an error.

## Contribute
Any help is welcome through PRs!


## License
[The GPLv3 license](https://www.gnu.org/licenses/gpl-3.0.en.html)



				//(item->encrypted) = (char*)encrypteee;
					// 7. save wallet
				
				
				
				/*ocall_status = ocall_save_encrypted_wallet(&ocall_ret, encrypteee, sizeof(encrypteee));
				free(sealed_data);
				if (ocall_ret != 0 || ocall_status != SGX_SUCCESS) {
					return ERR_CANNOT_SAVE_WALLET;
				}
				DEBUG_PRINT("[OK] Wallet successfully saved.");
				*/
				

			   //const sample_aes_gcm_128bit_key_t (*key)[16];
    			//key = &alice;
				/*int ressult;
				for(int b=0; wallet->items[b].certificate!='\0'; ++b){
					ressult =( ressult * 10) + (wallet->items[b].certificate) - '0';
				}
				uint8_t enckey=(uint8_t)ressult; 



					   			   //const sgx_aes_gcm_128bit_key_t alice = (atoi){wallet->items[i].certificate};

			   //uint8_t iv[12];
				//const sgx_aes_gcm_128bit_key_t (*key)[16];
				//key = &alice;
				//uint8_t *as = (uint8_t *)&item->certificate;
				// lenofatoi = sizeof(item->certificate);

								//gen_iv();
				//const sample_aes_gcm_128bit_key_t (*key)[16];
			   //const uint8_t* buf;

				//uint8_t *origMessage = (uint8_t *) decMessageIn;
				//uint8_t p_dst[lenOut];
				//sample_status_t ret;
			   //int iv = 12;
			   //const uint8_t* key;
			   //const uint8_t* tobeEncrypted;
			   //memcpy(&key,wallet->items[i].certificate,sizeof(wallet->items[i].certificate));
			   //memcpy(&tobeEncrypted,item->certificate,sizeof(item->certificate));
			   //key=(uint8_t)atoi(wallet->items[i].certificate);
			   //tobeEncrypted= (uint8_t)atoi(item->certificate);
			   //strncpy(key, wallet->items[i].certificate, sizeof(wallet->items[i].certificate));
			   //strncpy(tobeEncrypted, item->certificate,sizeof(item->certificate));


			   */
			   /*const sgx_aes_gcm_128bit_key_t alice = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf };

				int ress;
				for(int a=0; item->certificate[a]!='\0'; ++a){
					ress =ress * 10 + item->certificate[a] - '0';
				}
				uint8_t tobeEncrypted = (uint8_t)ress;
				uint8_t buf;
				uint32_t lenofTObe = sizeof(tobeEncrypted);
				uint32_t lenofiv = sizeof(iv);

			   	if(SGX_SUCCESS != sgx_rijndael128GCM_encrypt(&alice,
					&tobeEncrypted, lenofTObe, // plaintext
					&buf, // ciphertext
					&iv, // iv
					lenofiv, NULL, 0, // aad
					(sgx_aes_gcm_128bit_tag_t *) (buf) // mac
					))
			   break;
			   */


			   				//print_encr(bout);
			   /*
			   //return ERR_FAIL_SEAL;
			   uint8_t *plaintext = (uint8_t *)item->certificate;0x7f9276a897c0  0x7f961bda27c0
			   size_t aesgcm_len =4 + ((((double)sizee)/16))*16 +16;
			   item->encrypteee = (uint8_t*)malloc(aesgcm_len);
			   sgx_aes_gcm_128bit_tag_t mac;
			   const sgx_aes_gcm_128bit_key_t aes_key= { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf };
			   //sgx_read_rand((unsigned char *) &aes_key, sizeof(sgx_aes_gcm_128bit_key_t));
			   uint8_t iv[12];
			   memset(iv,0,12);
			   ((int*)item->encrypteee)[0]=sizee;
			   
			   sgx_status_t res;
			   	res= sgx_rijndael128GCM_encrypt(&aes_key, plaintext, sizee, (uint8_t*)item->encrypteee+4,iv,12 ,NULL,0,&mac);
				if (res != SGX_SUCCESS) {
					//printf("encryption error");
					free(wallet);
					return ERR_FAIL_UNSEAL;
				}
				item->encrypteee= '1';
				item->username[1]= 'a';
				*/


				//int ab = *(&item->encrypteee +1)-item->encrypteee;
				//memcpy(item->encrypted,item->encrypteee, ab);
				//(item->encrypted) =(char *) item->encrypteee;
				//return ERR_FAIL_SEAL;