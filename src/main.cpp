﻿#include <stdio.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <bitset>
#include <chrono>

#include "graphics.h"
#include "performance.h"
#include "positioning.h"

int main()
{

	positioning::generate_zone_masks();
	
	positioning::generate_movement_lookups();

	graphics::init_graphics_console();


	while (1)
	{


		positioning::game_state state = positioning::random_game_state();

		graphics::draw_on_console(state);

		positioning::possible_movements_cabeza moves = positioning::get_flaco_flat_moves(state, positioning::red_flaco_idx);
		for (int i = 0; i < 16; i++)
		{
			if (moves & (1 << i))
			{
				std::cout << positioning::kFlacoMovementBitName[i] << " ";
			}
		}
		std::cout << std::endl;

		getchar();
	}

	return 0;
}

/*
int main(){
	generate_zone_mask_cabeza();

	bitboard_t mask = zone_mask_cabeza[28][1][5];

	for(int y = 9; y >= 0; y--){
		for(int x = 0; x <= 9; x++){
			int bit = (mask >> (x + y*10))&1;
			if(bit){
				std::cout << 'x';
			}
			else{
				std::cout << '.';
			}
		}
		std::cout << std::endl;
	}

	getchar();

}

*/

/*
int main(){
	positioning::generate_movement_lookup_cabeza();

	char occupancy[3][3];

	for(int addr = 0x00; addr <= 0xFF; addr++){
		positioning::possible_movements moves = positioning::movement_lookup_cabeza[addr];

		occupancy[0][2] = addr&0x01 ? 'x' : '.';
		occupancy[0][1] = addr&0x02 ? 'x' : '.';
		occupancy[1][2] = addr&0x04 ? 'x' : '.';
		occupancy[1][1] = addr&0x08 ? 'x' : '.';
		occupancy[1][0] = addr&0x10 ? 'x' : '.';
		occupancy[2][2] = addr&0x20 ? 'x' : '.';
		occupancy[2][1] = addr&0x40 ? 'x' : '.';
		occupancy[2][0] = addr&0x80 ? 'x' : '.';
		occupancy[0][0] = 'O';

		for(int y = 2; y >= 0; y--){
			for(int x = 0; x <= 2; x++){
				std::cout << occupancy[x][y];
			}
			std::cout << std::endl;
		}
		std::cout << "Moves: ";

		std::cout << ((moves & POSSIBLE_MOVEMENT_U) ? "- U " : "    ");
		std::cout << ((moves & POSSIBLE_MOVEMENT_UR) ? "- UR " : "     ");
		std::cout << ((moves & POSSIBLE_MOVEMENT_R) ? "- R " : "    ");
		std::cout << ((moves & POSSIBLE_MOVEMENT_UU) ? "- UU " : "     ");
		std::cout << ((moves & POSSIBLE_MOVEMENT_UUR) ? "- UUR " : "      ");
		std::cout << ((moves & POSSIBLE_MOVEMENT_UURR) ? "- UURR " : "       ");
		std::cout << ((moves & POSSIBLE_MOVEMENT_URR) ? "- URR " : "      ");
		std::cout << ((moves & POSSIBLE_MOVEMENT_RR) ? "- RR " : "     ");


		std::cout << "------" << std::endl;
		getchar();

	}
}
*/

/*

	for(int i = 0; i < 10; i++){
	__int128_t occupancy = get_occupancy(state);

	for(int y = 0; y < 10; y++){
		for(int x = 0; x < 10; x++){
			if(occupancy&((__int128_t)1 << (y*10 + x))){
				std::cout << 'x';
			}
			else{
				std::cout << '.';
			}
		}
		std::cout << std::endl;
	}
	std::cout << "---------------------" << std::endl;
	getchar();
}
*/