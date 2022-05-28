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
int ecall_encrypt_item(const char* master_password, const item_t* item, const size_t item_size) {

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
	for(int i=0, i<MAX_ITEMS,i++){
		if(strcmp(wallet->items[i]->title==item->title) && strcmp(wallet->item[i]->title==item->username))
	}
	--wallet->size;
	DEBUG_PRINT("[OK] Item successfully removed.");

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
 * @brief      Adds an item to the wallet. The sizes/length of
 *             pointers need to be specified, otherwise SGX will
 *             assume a count of 1 for all pointers.
 *
 */
int ecall_add_item(const char* master_password, const item_t* item, const size_t item_size) {

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
