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
#include "enclave_t.h"
#include "string.h"
#include "stdint.h"

//#include "aes128gcm.h"
#include "sgx_tcrypto.h"
#include "sgx_trts.h"
//#include "libsgx_tcrypto.a"


#include "debug.h"
#include "enclave.h"
#include "wallet.h"

#include "sgx_tseal.h"
#include "sealing/sealing.h"

/**
 * @brief      Creates a new wallet with the provided master-password.
 *
 */
int ecall_create_wallet(const char* master_password) {

	//
	// OVERVIEW:
	//	1. check password policy
	//	2. [ocall] abort if wallet already exist
	//	3. create wallet
	//	4. seal wallet
	//	5. [ocall] save wallet
	//	6. exit enclave
	//
	//
	sgx_status_t ocall_status, sealing_status;
	int ocall_ret;

	DEBUG_PRINT("CREATING NEW WALLET...");


	// 1. check passaword policy
	if (strlen(master_password) < 8 || strlen(master_password)+1 > MAX_ITEM_SIZE) {
		return ERR_PASSWORD_OUT_OF_RANGE;
	}
	DEBUG_PRINT("[OK] Password policy successfully checked.");


	// 2. abort if wallet already exist
	ocall_status = ocall_is_wallet(&ocall_ret);
	if (ocall_ret != 0) {
		return ERR_WALLET_ALREADY_EXISTS;
	}
	DEBUG_PRINT("[OK] No pre-existing wallets.");


	// 3. create new wallet
	wallet_t* wallet = (wallet_t*)malloc(sizeof(wallet_t));
	wallet->size = 0;
	strncpy(wallet->master_password, master_password, strlen(master_password)+1);
	DEBUG_PRINT("[OK] New wallet successfully created.");


	// 4. seal wallet
	size_t sealed_size = sizeof(sgx_sealed_data_t) + sizeof(wallet_t);
	uint8_t* sealed_data = (uint8_t*)malloc(sealed_size);
    sealing_status = seal_wallet(wallet, (sgx_sealed_data_t*)sealed_data, sealed_size);
    free(wallet);
    if (sealing_status != SGX_SUCCESS) {
		free(sealed_data);
		return ERR_FAIL_SEAL;
    }
	DEBUG_PRINT("[OK] Seal wallet.");


	// 5. save wallet
	ocall_status = ocall_save_wallet(&ocall_ret, sealed_data, sealed_size);
	free(sealed_data);
	if (ocall_ret != 0 || ocall_status != SGX_SUCCESS) {
		return ERR_CANNOT_SAVE_WALLET;
	}
	DEBUG_PRINT("[OK] New wallet successfully saved.");


	// 6. exit enclave
	DEBUG_PRINT("WALLET SUCCESSFULLY CREATED.");
	return RET_SUCCESS;
}


/**
 * @brief      Provides the wallet content. The sizes/length of
 *             pointers need to be specified, otherwise SGX will
 *             assume a count of 1 for all pointers.
 *
 */
int ecall_show_wallet(const char* master_password, wallet_t* wallet, size_t wallet_size) {

	//
	// OVERVIEW:
	//	1. [ocall] load wallet
	// 	2. unseal wallet
	//	3. verify master-password
	//	4. return wallet to app
	//	5. exit enclave
	//
	//
	sgx_status_t ocall_status, sealing_status;
	int ocall_ret;

	DEBUG_PRINT("RETURNING WALLET TO APP...");


	// 1. load wallet
	size_t sealed_size = sizeof(sgx_sealed_data_t) + sizeof(wallet_t);
	uint8_t* sealed_data = (uint8_t*)malloc(sealed_size);
	ocall_status = ocall_load_wallet(&ocall_ret, sealed_data, sealed_size);
	if (ocall_ret != 0 || ocall_status != SGX_SUCCESS) {
		free(sealed_data);
		return ERR_CANNOT_LOAD_WALLET;
	}
	DEBUG_PRINT("[ok] Wallet successfully loaded.");


	// 2. unseal loaded wallet
	uint32_t plaintext_size = sizeof(wallet_t);
    wallet_t* unsealed_wallet = (wallet_t*)malloc(plaintext_size);
    sealing_status = unseal_wallet((sgx_sealed_data_t*)sealed_data, unsealed_wallet, plaintext_size);
    free(sealed_data);
    if (sealing_status != SGX_SUCCESS) {
		free(unsealed_wallet);
		return ERR_FAIL_UNSEAL;
    }
	DEBUG_PRINT("[OK] Unseal wallet.");


	// 3. verify master-password
	if (strcmp(unsealed_wallet->master_password, master_password) != 0) {
		free(unsealed_wallet);
		return ERR_WRONG_MASTER_PASSWORD;
	}
	DEBUG_PRINT("[ok] Master-password successfully verified.");


	// 4. return wallet to app
	(* wallet) = *unsealed_wallet;
	free(unsealed_wallet);
	DEBUG_PRINT("[ok] Wallet successfully saved to buffer.");


	// 5. exit enclave
	DEBUG_PRINT("WALLET SUCCESSFULLY RETURNED TO APP.");
	return RET_SUCCESS;
}


/**
 * @brief      Changes the wallet's master-password.
 *
 */
int ecall_change_master_password(const char* old_password, const char* new_password) {

	//
	// OVERVIEW:
	//	1. check password policy
	//	2. [ocall] load wallet
	// 	3. unseal wallet
	//	4. verify old password
	//	5. update password
	//	6. seal wallet
	// 	7. [ocall] save sealed wallet
	//	8. exit enclave
	//
	//
	sgx_status_t ocall_status, sealing_status;
	int ocall_ret;

	DEBUG_PRINT("CHANGING MASTER PASSWORD...");


	// 1. check passaword policy
	if (strlen(new_password) < 8 || strlen(new_password)+1 > MAX_ITEM_SIZE) {
		return ERR_PASSWORD_OUT_OF_RANGE;
	}
	DEBUG_PRINT("[ok] Password policy successfully checked.");


	// 2. load wallet
	size_t sealed_size = sizeof(sgx_sealed_data_t) + sizeof(wallet_t);
	uint8_t* sealed_data = (uint8_t*)malloc(sealed_size);
	ocall_status = ocall_load_wallet(&ocall_ret, sealed_data, sealed_size);
	if (ocall_ret != 0 || ocall_status != SGX_SUCCESS) {
		free(sealed_data);
		return ERR_CANNOT_LOAD_WALLET;
	}
	DEBUG_PRINT("[ok] Wallet successfully loaded.");


	// 3. unseal wallet
	uint32_t plaintext_size = sizeof(wallet_t);
    wallet_t* wallet = (wallet_t*)malloc(plaintext_size);
    sealing_status = unseal_wallet((sgx_sealed_data_t*)sealed_data, wallet, plaintext_size);
    free(sealed_data);
    if (sealing_status != SGX_SUCCESS) {
    	free(wallet);
		return ERR_FAIL_UNSEAL;
    }
	DEBUG_PRINT("[OK] Unseal wallet.");


	// 4. verify master-password
	if (strcmp(wallet->master_password, old_password) != 0) {
		free(wallet);
		return ERR_WRONG_MASTER_PASSWORD;
	}
	DEBUG_PRINT("[ok] Master-password successfully verified.");


	// 5. update password
	strncpy(wallet->master_password, new_password, strlen(new_password)+1);
	DEBUG_PRINT("[ok] Successfully updated master-password.");


	// 6. seal wallet
	sealed_data = (uint8_t*)malloc(sealed_size);
    sealing_status = seal_wallet(wallet, (sgx_sealed_data_t*)sealed_data, sealed_size);
    free(wallet);
    if (sealing_status != SGX_SUCCESS) {
			free(sealed_data);
			return ERR_FAIL_SEAL;
    }
	DEBUG_PRINT("[OK] Seal wallet.");


	// 7. save wallet
	ocall_status = ocall_save_wallet(&ocall_ret, sealed_data, sealed_size);
	free(sealed_data);
	if (ocall_ret != 0 || ocall_status != SGX_SUCCESS) {
		return ERR_CANNOT_SAVE_WALLET;
	}
	DEBUG_PRINT("[OK] Wallet successfully saved.");


	// 6. exit enclave
	DEBUG_PRINT("MASTER PASSWORD SUCCESSFULLY CHANGED.");
	return RET_SUCCESS;
}

/**
 * @brief      Adds an item to the wallet. The sizes/length of
 *             pointers need to be specified, otherwise SGX will
 *             assume a count of 1 for all pointers.
 *
 */
int ecall_encrypt_item(const char* master_password, item_t* item, const size_t item_size,const int sizee) {

	//
	// OVERVIEW:
	//	1. [ocall] load wallet
	//	2. unseal wallet
	//	3. verify master-password
	//	4. check input length
	//	5. add item to the wallet
	//	6. seal wallet
	//	7. [ocall] save sealed wallet
	//	8. exit enclave
	//
	//
	sgx_status_t ocall_status, sealing_status;
	int ocall_ret;

	DEBUG_PRINT("ADDING ITEM TO THE WALLET...");


	// 2. load wallet
	size_t sealed_size = sizeof(sgx_sealed_data_t) + sizeof(wallet_t);
	uint8_t* sealed_data = (uint8_t*)malloc(sealed_size);
	ocall_status = ocall_load_wallet(&ocall_ret, sealed_data, sealed_size);
	if (ocall_ret != 0 || ocall_status != SGX_SUCCESS) {
		free(sealed_data);
		return ERR_CANNOT_LOAD_WALLET;
	}
	DEBUG_PRINT("[ok] Wallet successfully loaded.");


	// 3. unseal wallet
	uint32_t plaintext_size = sizeof(wallet_t);
    wallet_t* wallet = (wallet_t*)malloc(plaintext_size);
    sealing_status = unseal_wallet((sgx_sealed_data_t*)sealed_data, wallet, plaintext_size);
    free(sealed_data);
    if (sealing_status != SGX_SUCCESS) {
    	free(wallet);
		return ERR_FAIL_UNSEAL;
    }
	DEBUG_PRINT("[OK] Unseal wallet.");


	// 3. verify master-password
	if (strcmp(wallet->master_password, master_password) != 0) {
		free(wallet);
		return ERR_WRONG_MASTER_PASSWORD;
	}
	DEBUG_PRINT("[ok] Master-password successfully verified.");


	// 4. check input length
	if (strlen(item->title)+1 > MAX_ITEM_SIZE ||
		strlen(item->username)+1 > MAX_ITEM_SIZE ||
		strlen(item->certificate)+1 > MAX_ITEM_SIZE
	) {
		free(wallet);
		return ERR_ITEM_TOO_LONG;
    }
	DEBUG_PRINT("[ok] Item successfully verified.");


	size_t wallet_size = wallet->size;
	for(int i=0; i<wallet_size; i++){
		if(strcmp(wallet->items[i].title, item->title) == 0 && 
		   strcmp(wallet->items[i].username, item->username) == 0 ){
			sgx_aes_gcm_128bit_tag_t mac;
			const sgx_aes_gcm_128bit_key_t aes_key= { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf };
			
			uint8_t iv[12];
			memset(iv,0,12);
				
			uint32_t encMessageLen = sizeof(iv)/sizeof(uint8_t)  + sizee;
			//const uint8_t* plaintext = 12; 
			uint8_t* plaintext = (uint8_t *)malloc(sizee);
			memcpy(plaintext,item->certificate, sizee * sizeof(uint8_t));
			wallet->items[0].encrypteee = (uint8_t*)malloc(encMessageLen+1);
			memset(wallet->items[0].encrypteee,0,encMessageLen+1);	
			uint32_t boutlen = *(&wallet->items[0].encrypteee +1)-wallet->items[0].encrypteee;
			uint32_t aes128gcm_ciphertext_size = sizeof(iv)/sizeof(uint8_t) + sizee;
			if(boutlen < aes128gcm_ciphertext_size)
				{
					return 0Xffffffff;
				}

			sgx_status_t res;
			res= sgx_rijndael128GCM_encrypt(&aes_key,
				plaintext, sizee, // plaintext
				wallet->items[0].encrypteee , iv, 12,// iv
				NULL, 0, // aad
				&mac);
			if (res != SGX_SUCCESS) {
				//printf("encryption error");
				free(wallet);
				return 0Xffffffff;
			}

			//item->encrypted=(char *) malloc((encMessageLen+1));
			memcpy(item->encrypted,(char *)wallet->items[0].encrypteee,encMessageLen*sizeof(uint8_t));

			item->encrypteee = (uint8_t*)malloc(sizeof(wallet->items[0].encrypteee)/sizeof(uint8_t));
			memcpy(item->encrypteee,wallet->items[0].encrypteee,encMessageLen*sizeof(uint8_t));
			

			uint32_t ciphertextlen = encMessageLen -(sizeof(iv)/sizeof(uint8_t));
			//uint32_t encsize= sizeof(bout)/sizeof(uint8_t);
			uint8_t *plaintext1= (uint8_t*) malloc(sizee * sizeof(uint8_t));
			memset(plaintext1,0,sizee * sizeof(uint8_t));
			size_t plainlen = sizeof(plaintext1)/sizeof(uint8_t);
			sgx_status_t rest;

			rest= sgx_rijndael128GCM_decrypt(&aes_key, wallet->items[0].encrypteee,
			ciphertextlen, plaintext1, iv ,12 ,NULL,0,
			&mac);
			if (rest != SGX_SUCCESS) {
				//printf("encryption error");
				free(wallet);
				return 0Xffffffff;
			}
			memset(item->decrypted,0,sizee);
			memcpy(item->decrypted,(char*)plaintext1,sizee+1);

			//item->decrypted= &as;
			//free(bout);
			free(plaintext1);
			//plaintext=NULL;
			free(plaintext);
			

			DEBUG_PRINT("[OK] Item successfully added.");
			sealed_data = (uint8_t*)malloc(sealed_size);

			//free(item);
			sealing_status = seal_wallet(wallet, (sgx_sealed_data_t*)sealed_data, sealed_size);
			free(wallet);
			if (sealing_status != SGX_SUCCESS) {
				free(wallet);
				free(sealed_data);
				return ERR_FAIL_SEAL;
			}
			DEBUG_PRINT("[OK] Seal wallet.");


			// 7. save wallet
			ocall_status = ocall_save_wallet(&ocall_ret, sealed_data, sealed_size);
			free(sealed_data);
			if (ocall_ret != 0 || ocall_status != SGX_SUCCESS) {
				return ERR_CANNOT_SAVE_WALLET;
			}
			DEBUG_PRINT("[OK] Wallet successfully saved.");


			// 8. exit enclave
			DEBUG_PRINT("ITEM SUCCESSFULLY ADDED TO THE WALLET.");
			return RET_SUCCESS;
			break;
			   
		}else{
			return ERR_CANNOT_LOAD_WALLET;
		}
	}
}

int ecall_decrypt_item(const char* master_password, item_t* item, const size_t item_size,const int sizee) {

	//
	// OVERVIEW:
	//	1. [ocall] load wallet
	//	2. unseal wallet
	//	3. verify master-password
	//	4. check input length
	//	5. add item to the wallet
	//	6. seal wallet
	//	7. [ocall] save sealed wallet
	//	8. exit enclave
	//
	//
	sgx_status_t ocall_status, sealing_status;
	int ocall_ret;

	DEBUG_PRINT("ADDING ITEM TO THE WALLET...");


	// 2. load wallet
	size_t sealed_size = sizeof(sgx_sealed_data_t) + sizeof(wallet_t);
	uint8_t* sealed_data = (uint8_t*)malloc(sealed_size);
	ocall_status = ocall_load_wallet(&ocall_ret, sealed_data, sealed_size);
	if (ocall_ret != 0 || ocall_status != SGX_SUCCESS) {
		free(sealed_data);
		return ERR_CANNOT_LOAD_WALLET;
	}
	DEBUG_PRINT("[ok] Wallet successfully loaded.");


	// 3. unseal wallet
	uint32_t plaintext_size = sizeof(wallet_t);
    wallet_t* wallet = (wallet_t*)malloc(plaintext_size);
    sealing_status = unseal_wallet((sgx_sealed_data_t*)sealed_data, wallet, plaintext_size);
    free(sealed_data);
    if (sealing_status != SGX_SUCCESS) {
    	free(wallet);
		return ERR_FAIL_UNSEAL;
    }
	DEBUG_PRINT("[OK] Unseal wallet.");


	// 3. verify master-password
	if (strcmp(wallet->master_password, master_password) != 0) {
		free(wallet);
		return ERR_WRONG_MASTER_PASSWORD;
	}
	DEBUG_PRINT("[ok] Master-password successfully verified.");


	// 4. check input length
	if (strlen(item->title)+1 > MAX_ITEM_SIZE ||
		strlen(item->username)+1 > MAX_ITEM_SIZE ||
		strlen(item->certificate)+1 > MAX_ITEM_SIZE
	) {
		free(wallet);
		return ERR_ITEM_TOO_LONG;
    }
	DEBUG_PRINT("[ok] Item successfully verified.");


	sgx_aes_gcm_128bit_tag_t mac;
	const sgx_aes_gcm_128bit_key_t aes_key= { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf };

	uint8_t iv[12];
	memset(iv,0,12);
		
	uint32_t encMessageLen = sizeof(iv)/sizeof(uint8_t)  + sizee;
	//const uint8_t* plaintext = 12; 
	uint8_t* plaintext = (uint8_t *)malloc(sizee);
	memcpy(plaintext,item->certificate, sizee * sizeof(uint8_t));
	item->encrypteee = (uint8_t*)malloc(encMessageLen+1);
	memset(item->encrypteee,0,encMessageLen+1);	
	uint32_t boutlen = *(&item->encrypteee +1)-item->encrypteee;
	uint32_t aes128gcm_ciphertext_size = sizeof(iv)/sizeof(uint8_t) + sizee;
	if(boutlen < aes128gcm_ciphertext_size)
		{
			return 0Xffffffff;
		}

	sgx_status_t res;
	res= sgx_rijndael128GCM_encrypt(&aes_key,
		plaintext, sizee, // plaintext
		item->encrypteee , iv, 12,// iv
		NULL, 0, // aad
		&mac);
	if (res != SGX_SUCCESS) {
		//printf("encryption error");
		free(wallet);
		return 0Xffffffff;
	}

		//item->encrypted=(char *) malloc((encMessageLen+1));
		

		uint32_t ciphertextlen = encMessageLen -(sizeof(iv)/sizeof(uint8_t));
		//uint32_t encsize= sizeof(bout)/sizeof(uint8_t);
		uint8_t *plaintext1= (uint8_t*) malloc(sizee * sizeof(uint8_t));
		memset(plaintext1,0,sizee * sizeof(uint8_t));
		size_t plainlen = sizeof(plaintext1)/sizeof(uint8_t);
		sgx_status_t rest;

		rest= sgx_rijndael128GCM_decrypt(&aes_key, wallet->items[0].encrypteee,
		ciphertextlen, plaintext1, iv ,12 ,NULL,0,
		&mac);
		if (rest != SGX_SUCCESS) {
			//printf("encryption error");
			free(wallet);
			return 0Xffffffff;
		}
		//wallet->items[0].decrypted = 0;
		memset(wallet->items[0].decrypted,0,sizee+1);
		memcpy(wallet->items[0].decrypted,(char*)plaintext1,sizee+1);

		free(plaintext1);
		free(plaintext);

	DEBUG_PRINT("[OK] Item successfully added.");
	sealed_data = (uint8_t*)malloc(sealed_size);


	sealing_status = seal_wallet(wallet, (sgx_sealed_data_t*)sealed_data, sealed_size);
	free(wallet);
	if (sealing_status != SGX_SUCCESS) {
		free(wallet);
		free(sealed_data);
		return ERR_FAIL_SEAL;
	}
	DEBUG_PRINT("[OK] Seal wallet.");


	// 7. save wallet
	ocall_status = ocall_save_wallet(&ocall_ret, sealed_data, sealed_size);
	free(sealed_data);
	if (ocall_ret != 0 || ocall_status != SGX_SUCCESS) {
		return ERR_CANNOT_SAVE_WALLET;
	}
	DEBUG_PRINT("[OK] Wallet successfully saved.");


	// 8. exit enclave
	DEBUG_PRINT("ITEM SUCCESSFULLY ADDED TO THE WALLET.");
	return RET_SUCCESS;

}

int ecall_token(const char* master_password, item_t* item, const size_t item_size) {

	//
	// OVERVIEW:
	//	1. [ocall] load wallet
	//	2. unseal wallet
	//	3. verify master-password
	//	4. check input length
	//	5. add item to the wallet
	//	6. seal wallet
	//	7. [ocall] save sealed wallet
	//	8. exit enclave
	//
	//
	sgx_status_t ocall_status, sealing_status;
	int ocall_ret;

	DEBUG_PRINT("ADDING ITEM TO THE WALLET...");


	// 2. load wallet
	size_t sealed_size = sizeof(sgx_sealed_data_t) + sizeof(wallet_t);
	uint8_t* sealed_data = (uint8_t*)malloc(sealed_size);
	ocall_status = ocall_load_wallet(&ocall_ret, sealed_data, sealed_size);
	if (ocall_ret != 0 || ocall_status != SGX_SUCCESS) {
		free(sealed_data);
		return ERR_CANNOT_LOAD_WALLET;
	}
	DEBUG_PRINT("[ok] Wallet successfully loaded.");


	// 3. unseal wallet
	uint32_t plaintext_size = sizeof(wallet_t);
    wallet_t* wallet = (wallet_t*)malloc(plaintext_size);
    sealing_status = unseal_wallet((sgx_sealed_data_t*)sealed_data, wallet, plaintext_size);
    free(sealed_data);
    if (sealing_status != SGX_SUCCESS) {
    	free(wallet);
		return ERR_FAIL_UNSEAL;
    }
	DEBUG_PRINT("[OK] Unseal wallet.");


	// 3. verify master-password
	if (strcmp(wallet->master_password, master_password) != 0) {
		free(wallet);
		return ERR_WRONG_MASTER_PASSWORD;
	}
	DEBUG_PRINT("[ok] Master-password successfully verified.");


	// 4. check input length
	if (strlen(item->title)+1 > MAX_ITEM_SIZE ||
		strlen(item->username)+1 > MAX_ITEM_SIZE ||
		strlen(item->certificate)+1 > MAX_ITEM_SIZE
	) {
		free(wallet);
		return ERR_ITEM_TOO_LONG;
    }
	DEBUG_PRINT("[ok] Item successfully verified.");
	
	//memset(item->encrypteee, 0,MAX_ITEM_SIZE);
	
	for(int i=0; i<7; i++){
		if(i==6){
			wallet->items[0].nadratoken[i]='\0';
		}else{
		wallet->items[0].nadratoken[i]= item->nadratoken[i];
		}
	}		


	// 5. add item to the wallet
	size_t wallet_size = wallet->size;
	if (wallet_size >= MAX_ITEMS) {
		free(wallet);
		return ERR_WALLET_FULL;
	}

	DEBUG_PRINT("[OK] Item successfully added.");


	// 6. seal wallet
	sealed_data = (uint8_t*)malloc(sealed_size);
    sealing_status = seal_wallet(wallet, (sgx_sealed_data_t*)sealed_data, sealed_size);
    free(wallet);
    if (sealing_status != SGX_SUCCESS) {
    	free(wallet);
		free(sealed_data);
		return ERR_FAIL_SEAL;
    }
	DEBUG_PRINT("[OK] Seal wallet.");


	// 7. save wallet
	ocall_status = ocall_save_wallet(&ocall_ret, sealed_data, sealed_size);
	free(sealed_data);
	if (ocall_ret != 0 || ocall_status != SGX_SUCCESS) {
		return ERR_CANNOT_SAVE_WALLET;
	}
	DEBUG_PRINT("[OK] Wallet successfully saved.");


	// 8. exit enclave
	DEBUG_PRINT("ITEM SUCCESSFULLY ADDED TO THE WALLET.");
	return RET_SUCCESS;
	
		
}




/**
 * @brief      Adds an item to the wallet. The sizes/length of
 *             pointers need to be specified, otherwise SGX will
 *             assume a count of 1 for all pointers.
 *
 */
int ecall_add_item( char* master_password,  item_t* item,  size_t item_size) {

	//
	// OVERVIEW:
	//	1. [ocall] load wallet
	//	2. unseal wallet
	//	3. verify master-password
	//	4. check input length
	//	5. add item to the wallet
	//	6. seal wallet
	//	7. [ocall] save sealed wallet
	//	8. exit enclave
	//
	//
	sgx_status_t ocall_status, sealing_status;
	int ocall_ret;

	DEBUG_PRINT("ADDING ITEM TO THE WALLET...");


	// 2. load wallet
	size_t sealed_size = sizeof(sgx_sealed_data_t) + sizeof(wallet_t);
	uint8_t* sealed_data = (uint8_t*)malloc(sealed_size);
	ocall_status = ocall_load_wallet(&ocall_ret, sealed_data, sealed_size);
	if (ocall_ret != 0 || ocall_status != SGX_SUCCESS) {
		free(sealed_data);
		return ERR_CANNOT_LOAD_WALLET;
	}
	DEBUG_PRINT("[ok] Wallet successfully loaded.");


	// 3. unseal wallet
	uint32_t plaintext_size = sizeof(wallet_t);
    wallet_t* wallet = (wallet_t*)malloc(plaintext_size);
    sealing_status = unseal_wallet((sgx_sealed_data_t*)sealed_data, wallet, plaintext_size);
    free(sealed_data);
    if (sealing_status != SGX_SUCCESS) {
    	free(wallet);
		return ERR_FAIL_UNSEAL;
    }
	DEBUG_PRINT("[OK] Unseal wallet.");


	// 3. verify master-password
	if (strcmp(wallet->master_password, master_password) != 0) {
		free(wallet);
		return ERR_WRONG_MASTER_PASSWORD;
	}
	DEBUG_PRINT("[ok] Master-password successfully verified.");


	// 4. check input length
	if (strlen(item->title)+1 > MAX_ITEM_SIZE ||
		strlen(item->username)+1 > MAX_ITEM_SIZE ||
		strlen(item->certificate)+1 > MAX_ITEM_SIZE
	) {
		free(wallet);
		return ERR_ITEM_TOO_LONG;
    }
	DEBUG_PRINT("[ok] Item successfully verified.");
	
	//memset(item->encrypteee, 0,MAX_ITEM_SIZE);
	

	item->encrypteee=NULL;
	item->nadratoken[0]='\0';
	for(int i=0; i<7; i++){
		
		item->decrypted[i]='\0';
		item->encrypted[i]='\0';
	}

	// 5. add item to the wallet
	size_t wallet_size = wallet->size;
	if (wallet_size >= MAX_ITEMS) {
		free(wallet);
		return ERR_WALLET_FULL;
	}
	wallet->items[wallet_size] = *item;
	++wallet->size;
	DEBUG_PRINT("[OK] Item successfully added.");


	// 6. seal wallet
	sealed_data = (uint8_t*)malloc(sealed_size);
    sealing_status = seal_wallet(wallet, (sgx_sealed_data_t*)sealed_data, sealed_size);
    free(wallet);
    if (sealing_status != SGX_SUCCESS) {
    	free(wallet);
		free(sealed_data);
		return ERR_FAIL_SEAL;
    }
	DEBUG_PRINT("[OK] Seal wallet.");


	// 7. save wallet
	ocall_status = ocall_save_wallet(&ocall_ret, sealed_data, sealed_size);
	free(sealed_data);
	if (ocall_ret != 0 || ocall_status != SGX_SUCCESS) {
		return ERR_CANNOT_SAVE_WALLET;
	}
	DEBUG_PRINT("[OK] Wallet successfully saved.");


	// 8. exit enclave
	DEBUG_PRINT("ITEM SUCCESSFULLY ADDED TO THE WALLET.");
	return RET_SUCCESS;
}


/**
 * @brief      Removes an item from the wallet. The sizes/length of
 *             pointers need to be specified, otherwise SGX will
 *             assume a count of 1 for all pointers.
 *
 */
int ecall_remove_item(const char* master_password, const int index) {

	//
	// OVERVIEW:
	//	1. check index bounds
	//	2. [ocall] load wallet
	//	3. unseal wallet
	//	4. verify master-password
	//	5. remove item from the wallet
	//	6. seal wallet
	//	7. [ocall] save sealed wallet
	//	8. exit enclave
	//
	//
	sgx_status_t ocall_status, sealing_status;
	int ocall_ret;

	DEBUG_PRINT("REMOVING ITEM FROM THE WALLET...");


	// 1. check index bounds
	if (index < 0 || index >= MAX_ITEMS) {
		return ERR_ITEM_DOES_NOT_EXIST;
	}
	DEBUG_PRINT("[OK] Successfully checked index bounds.");


	// 2. load wallet
	size_t sealed_size = sizeof(sgx_sealed_data_t) + sizeof(wallet_t);
	uint8_t* sealed_data = (uint8_t*)malloc(sealed_size);
	ocall_status = ocall_load_wallet(&ocall_ret, sealed_data, sealed_size);
	if (ocall_ret != 0 || ocall_status != SGX_SUCCESS) {
		free(sealed_data);
		return ERR_CANNOT_LOAD_WALLET;
	}
	DEBUG_PRINT("[ok] Wallet successfully loaded.");


	// 3. unseal wallet
	uint32_t plaintext_size = sizeof(wallet_t);
    wallet_t* wallet = (wallet_t*)malloc(plaintext_size);
    sealing_status = unseal_wallet((sgx_sealed_data_t*)sealed_data, wallet, plaintext_size);
    free(sealed_data);
    if (sealing_status != SGX_SUCCESS) {
    	free(wallet);
		return ERR_FAIL_UNSEAL;
    }
	DEBUG_PRINT("[OK] Unseal wallet.");


	// 4. verify master-password
	if (strcmp(wallet->master_password, master_password) != 0) {
		free(wallet);
		return ERR_WRONG_MASTER_PASSWORD;
	}
	DEBUG_PRINT("[ok] Master-password successfully verified.");


	// 5. remove item from the wallet
	size_t wallet_size = wallet->size;
	if (index >= wallet_size) {
		free(wallet);
		return ERR_ITEM_DOES_NOT_EXIST;
	}
	for (int i = index; i < wallet_size-1; ++i) {
		wallet->items[i] = wallet->items[i+1];
	}
	--wallet->size;
	DEBUG_PRINT("[OK] Item successfully removed.");


	// 6. seal wallet
	sealed_data = (uint8_t*)malloc(sealed_size);
    sealing_status = seal_wallet(wallet, (sgx_sealed_data_t*)sealed_data, sealed_size);
    free(wallet);
    if (sealing_status != SGX_SUCCESS) {
		free(sealed_data);
		return ERR_FAIL_SEAL;
    }
	DEBUG_PRINT("[OK] Seal wallet.");


	// 7. save wallet
	ocall_status = ocall_save_wallet(&ocall_ret, sealed_data, sealed_size);
	free(sealed_data);
	if (ocall_ret != 0 || ocall_status != SGX_SUCCESS) {
		return ERR_CANNOT_SAVE_WALLET;
	}
	DEBUG_PRINT("[OK] Wallet successfully saved.");


	// 8. exit enclave
	DEBUG_PRINT("ITEM SUCCESSFULLY REMOVED FROM THE WALLET.");
	return RET_SUCCESS;
}
