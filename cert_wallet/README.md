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




								//free(buout);
				
			   /*
			   uint32_t encMessageLen = SGX_AESGCM_IV_SIZE + SGX_AESGCM_MAC_SIZE + sizee;
			   bout=(uint8_t*)malloc(encMessageLen);
			   
			   
			   uint32_t boutlen = *(&bout +1)-bout;
			   //uint32_t boutlen = sizeof(item->encrypteee);
			   uint32_t aes128gcm_ciphertext_size = SGX_AESGCM_IV_SIZE + SGX_AESGCM_MAC_SIZE + sizee;
			   if(boutlen < aes128gcm_ciphertext_size)
					{
						return 0Xffffffff;
					}
			   
			   	if(sgx_read_rand(bout, SGX_AESGCM_IV_SIZE) != SGX_SUCCESS)
					{
						return 0Xffffffff;
					}
				const sgx_aes_gcm_128bit_key_t aes_key= { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xd, 0xa };
				uint8_t *plaintext = (uint8_t *)item->certificate;
			    sgx_status_t res;
			   	res=sgx_rijndael128GCM_encrypt(&aes_key,
					plaintext, sizee, // plaintext
					bout + SGX_AESGCM_IV_SIZE + SGX_AESGCM_MAC_SIZE, // ciphertext
					bout, SGX_AESGCM_IV_SIZE, // iv
					NULL, 0, // aad
					(sgx_aes_gcm_128bit_tag_t*) (bout + SGX_AESGCM_IV_SIZE)); // mac
				if (res != SGX_SUCCESS) {
					//printf("encryption error");
					free(wallet);
					return 0Xffffffff;
				}	
				item->encrypted=(unsigned char *) malloc((sizee)*sizeof(unsigned char));
				memcpy(item->encrypted,bout,sizee);
				item->encrypteee = bout;		
				*/




			   	// sgx_aes_gcm_128bit_tag_t mac;
			    // const sgx_aes_gcm_128bit_key_t aes_key= { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf };

				// uint8_t iv[12];
			   	// memset(iv,0,12);
				   
			   	// uint32_t encMessageLen = SGX_AESGCM_IV_SIZE + SGX_AESGCM_MAC_SIZE  + sizee;
			    // //const uint8_t* plaintext = 12; 
			   	// uint8_t* plaintext = (uint8_t *)malloc(sizee);
				// memcpy(plaintext,item->certificate, sizee * sizeof(uint8_t));
			   	// uint8_t* bout = (uint8_t*)malloc(encMessageLen+1);
				// memset(bout,0,encMessageLen+1);	
				// uint32_t boutlen = *(&bout +1)-bout;
				// uint32_t aes128gcm_ciphertext_size = SGX_AESGCM_IV_SIZE + SGX_AESGCM_MAC_SIZE + sizee;
			   	// if(boutlen < aes128gcm_ciphertext_size)
				// 	{
				// 		return 0Xffffffff;
				// 	}
				// if(sgx_read_rand(bout, SGX_AESGCM_IV_SIZE) != SGX_SUCCESS)
				// 	{
				// 		return 0Xffffffff;
				// 	}
			   	// sgx_status_t res;
			   	// res= sgx_rijndael128GCM_encrypt(&aes_key,
				// 	plaintext, sizee, // plaintext
				// 	bout + SGX_AESGCM_IV_SIZE + SGX_AESGCM_MAC_SIZE, // ciphertext
				// 	bout, SGX_AESGCM_IV_SIZE, // iv
				// 	NULL, 0, // aad
				// 	(sgx_aes_gcm_128bit_tag_t*) (bout + SGX_AESGCM_IV_SIZE));
				// if (res != SGX_SUCCESS) {
				// 	//printf("encryption error");
				// 	free(wallet);
				// 	return 0Xffffffff;
				// }

				// //item->encrypted=(char *) malloc((encMessageLen+1));
				// memcpy(item->encrypted,(char *)bout,encMessageLen*sizeof(uint8_t));

				// item->encrypteee = (uint8_t*)malloc(sizeof(bout)/sizeof(uint8_t));
				// memcpy(item->encrypteee,bout,encMessageLen*sizeof(uint8_t));
				

				// uint32_t ciphertextlen = encMessageLen -(SGX_AESGCM_IV_SIZE + SGX_AESGCM_MAC_SIZE);
				// uint32_t encsize= sizeof(bout)/sizeof(uint8_t);
				// uint8_t *plaintext1= (uint8_t*) malloc(sizee * sizeof(uint8_t));
				// memset(plaintext1,0,sizee * sizeof(uint8_t));
				// size_t plainlen = sizeof(plaintext1)/sizeof(uint8_t);
				// sgx_status_t rest;

				// rest= sgx_rijndael128GCM_decrypt(&aes_key, bout + SGX_AESGCM_IV_SIZE + SGX_AESGCM_MAC_SIZE,
				// ciphertextlen, plaintext1, bout ,SGX_AESGCM_IV_SIZE ,NULL,0,
				// (sgx_aes_gcm_128bit_tag_t*) (bout + SGX_AESGCM_IV_SIZE));
				// if (rest != SGX_SUCCESS) {
				// 	//printf("encryption error");
				// 	free(wallet);
				// 	return ERR_FAIL_UNSEAL;
				// }
				// item->decrypteee = (uint8_t*) malloc((sizee));
				// memcpy(item->decrypteee,plaintext1,sizee);
				// //char as = 'M';
				// //item->decrypted = ( char *) malloc((sizee)*sizeof( char));
				// //item->decrypted=NULL;

				// memcpy(item->decrypted,(char*)plaintext1,sizee);
				// item->decrypted[MAX_ITEM_SIZE-1]='\0';
				// item->decrypted[MAX_ITEM_SIZE-2]='\0';
				// //item->decrypted= &as;
				// free(bout);
				// free(plaintext1);
				// //plaintext=NULL;
				// free(plaintext);



require_once './db.php';
require_once './helper.php';
$username = isset($_GET['username']) ? $_GET['username'] : '';
$signature = isset($_GET['signature']) ? $_GET['signature'] : '';
$granttoken = isset($_GET['granttoken']) ? $_GET['granttoken'] : '';
echo "<pre>username : {$username}<br/>signature : {$signature}</pre>";  
if($username != '' && $signature != '' && $granttoken!= '' ) {
    $data = $db->get("SELECT * from nadraid where username = '{$username}' and signature = '{$signature}' and token IS NOT NULL" );
    if($data) {
        echo 'ACCOUNT_ALREADY_REGISTERED';
    }
    else {
    
        $data = $db->get("SELECT * from nadraid where username = '{$username}' and signature = '{$signature}'" );
	if($data) { 
		$token = RandomString(6);
		$status = $db->get("UPDATE nadraid SET  token='{$token}', granttoken='{$granttoken}' where username = '{$username}' and signature = '{$signature}'" );
		if($status == true) {
		        header("x-accesstoken:{$token}");
		        header("x-granttoken:{$granttoken}");
		        echo 'ACCOUNT_REGISTRATION_SUCCESSFULL';
		    
		}
		else {
		        echo 'ACCOUNT_REGISTRATION_FAILED';
		}
	}
	else {
	        echo 'ACCOUNT_NOT_FOUND';
	}
    
    
    }
}
else {
    echo 'MISSING_PARAMETERS';
}