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
#include <stdint.h>
#ifndef WALLET_H_
#define WALLET_H_


/***************************************************
 * Defines
 ***************************************************/
#define MAX_ITEMS 100
#define MAX_ITEM_SIZE 10


/***************************************************
 * Struct
 ***************************************************/
// item
struct Item {
	char  title[8];
	char  username[MAX_ITEM_SIZE];
	char  certificate[MAX_ITEM_SIZE];
	char encrypted[8];
	uint8_t* encrypteee;
	char decrypted[8];
	char nadratoken[7];
};
typedef struct Item item_t;

// wallet
struct Wallet {
	item_t items[MAX_ITEMS];
	size_t size;
	char master_password[MAX_ITEM_SIZE];
	//char encrypted[MAX_ITEM_SIZE];
};
typedef struct Wallet wallet_t;


#endif // WALLET_H_