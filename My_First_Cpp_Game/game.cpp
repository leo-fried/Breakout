#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

float player_1_p, player_1_dp, player_p, player_dp;
float arena_half_size_x = 85, arena_half_size_y = 45;
float player_half_size_x = 2.5, player_half_size_y = 10;
float ball_p_x, ball_p_y = -35, ball_dp_x, ball_dp_y = 0, ball_half_size = 1;

int player_score;
int player_lives = 3;
int easySquares = 30;
int mediumSquares = 60;
int hardSquares = 120;
unsigned int time = 50000;

internal void
simulate_player(float* p, float* dp, float ddp, float dt) {
	ddp -= *dp * 10.f;

	*p = *p + *dp * dt + ddp * dt * dt * .5f;
	*dp = *dp + ddp * dt;

	if (*p + player_half_size_x > arena_half_size_x - 17.5) {
		*p = (arena_half_size_x - 17.5) - player_half_size_x;
		*dp = 0;
	}
	else if (*p - player_half_size_x < -arena_half_size_x + 17.5) {
		*p = (-arena_half_size_x + 17.5) + player_half_size_x;
		*dp = 0;
	}
}

internal bool
aabb_vs_aabb(float p1x, float p1y, float hs1x, float hs1y,
	float p2x, float p2y, float hs2x, float hs2y) {
	return (p1x + hs1x > p2x - hs2x &&
		p1x - hs1x < p2x + hs2x &&
		p1y + hs1y > p2y - hs2y &&
		p1y + hs1y < p2y + hs2y);
}

internal void
check_rect_hit(int y1, int y2, int x1, int x2, int arr[][10], int index, int index2, int& c)
{
	if ((ball_p_y + ball_half_size >= y1 && ball_p_y + ball_half_size <= y2) && (ball_p_x + ball_half_size >= x1 && ball_p_x + ball_half_size <= x2) && arr[index][index2] == 0) {
		player_score += 50;
		ball_dp_y *= -1;
		arr[index][index2] = 1;
		c--;
	}
}

enum Gamemode {
	GM_MENU,
	GM_GAMEPLAY,
	GM_VICTORY,
};

Gamemode current_gamemode;
int hot_button;

internal void
simulate_game(Input* input, float dt) {
	if (hot_button == 0)
	{
		draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0XA2dEE8);
		draw_arena_borders(arena_half_size_y + 30, arena_half_size_x, 0x048EA4);
		draw_arena_borders(arena_half_size_x, arena_half_size_y, 0x048EA4);
		draw_rect(0, -48, 75, 3, 0XA2dEE8);
	}
	else if (hot_button == 1)
	{
		draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0xffc100);
		draw_arena_borders(arena_half_size_y + 30, arena_half_size_x, 0xcb9807);
		draw_arena_borders(arena_half_size_x, arena_half_size_y, 0xcb9807);
		draw_rect(0, -48, 75, 3, 0xffc100);
	}
	else
	{
		draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
		draw_arena_borders(arena_half_size_y + 30, arena_half_size_x, 0x5f5f5f);
		draw_arena_borders(arena_half_size_x, arena_half_size_y, 0x5f5f5f);
		draw_rect(0, -48, 75, 3, 0x000000);
	}



	if (current_gamemode == GM_GAMEPLAY) {
		float player_ddp = 0.f;
		if (is_down(BUTTON_A)) player_ddp -= 2000;
		if (is_down(BUTTON_D)) player_ddp += 2000;
		simulate_player(&player_p, &player_dp, player_ddp, dt);


		// Simulate Ball
		{
			ball_p_x += ball_dp_x * dt;
			ball_p_y += ball_dp_y * dt;

			if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, player_p, -50, player_half_size_x + 7, player_half_size_y)) {
				ball_p_y = -50 + player_half_size_y + ball_half_size;
				ball_dp_y *= -1;
				ball_dp_x = (ball_p_x - player_p) * 2 + player_dp * .75f;
			}


			if (ball_p_y + ball_half_size > arena_half_size_y) {
				ball_p_y = arena_half_size_y - ball_half_size;
				ball_dp_y *= -1;

			}
			if (ball_p_x + ball_half_size > arena_half_size_x - 10) {
				ball_p_x = arena_half_size_x - 10 - ball_half_size;
				ball_dp_x *= -1;
			}
			if (ball_p_x - ball_half_size < -arena_half_size_x + 10) {
				ball_p_x = -arena_half_size_x + 10 + ball_half_size;
				ball_dp_x *= -1;

			}
			if (ball_p_y - ball_half_size < -arena_half_size_y) {
				ball_dp_x = 0;
				ball_dp_y *= 0;
				ball_p_x = 0;
				ball_p_y = -35;
				if(easySquares != 0 || mediumSquares != 0 || hardSquares != 0) player_lives--;
			}
		}
		// Changed to 1 if area is hit
		static int drawLogic[3][10] = {
			{0, 0, 0 ,0 ,0 ,0 ,0, 0, 0, 0},
			{ 0, 0, 0 ,0 ,0 ,0 ,0, 0, 0, 0},
			{0, 0, 0 ,0 ,0 ,0 ,0, 0, 0, 0},
		};

		static int drawLogic2[6][10] = {
			{0, 0, 0 ,0 ,0 ,0 ,0, 0, 0, 0},
			{ 0, 0, 0 ,0 ,0 ,0 ,0, 0, 0, 0},
			{0, 0, 0 ,0 ,0 ,0 ,0, 0, 0, 0},
			{0, 0, 0 ,0 ,0 ,0 ,0, 0, 0, 0},
			{ 0, 0, 0 ,0 ,0 ,0 ,0, 0, 0, 0},
			{0, 0, 0 ,0 ,0 ,0 ,0, 0, 0, 0},
		};
		static int drawLogic3[12][10] = {
			{0, 0, 0 ,0 ,0 ,0 ,0, 0, 0, 0},
			{ 0, 0, 0 ,0 ,0 ,0 ,0, 0, 0, 0},
			{0, 0, 0 ,0 ,0 ,0 ,0, 0, 0, 0},
			{0, 0, 0 ,0 ,0 ,0 ,0, 0, 0, 0},
			{ 0, 0, 0 ,0 ,0 ,0 ,0, 0, 0, 0},
			{0, 0, 0 ,0 ,0 ,0 ,0, 0, 0, 0},
			{0, 0, 0 ,0 ,0 ,0 ,0, 0, 0, 0},
			{ 0, 0, 0 ,0 ,0 ,0 ,0, 0, 0, 0},
			{0, 0, 0 ,0 ,0 ,0 ,0, 0, 0, 0},
			{0, 0, 0 ,0 ,0 ,0 ,0, 0, 0, 0},
			{ 0, 0, 0 ,0 ,0 ,0 ,0, 0, 0, 0},
			{0, 0, 0 ,0 ,0 ,0 ,0, 0, 0, 0},
		};


		int colors[6] = { 
			0xff0000, //Red
			0xff7c00, //Orange
			0xf7ff00, //Yellow
			0x68cf00, //Green
			0x0087cf, //Blue
			0x9d49f7  //Purple
		};

		if (player_lives <= 0)
		{
			draw_rect(0, 0, arena_half_size_x, arena_half_size_y + 10, 0XFF0000);
			draw_text("GAME OVER", -50, 20, 2, 0xffffff);
			for (int i = 0; i < 10; i++)
			{
				//RESET DRAW LOGIC
				drawLogic[0][i] = 0;
				drawLogic[1][i] = 0;
				drawLogic[2][i] = 0;
				
				drawLogic2[0][i] = 0;
				drawLogic2[1][i] = 0;
				drawLogic2[2][i] = 0;
				drawLogic2[3][i] = 0;
				drawLogic2[4][i] = 0;
				drawLogic2[5][i] = 0;

				drawLogic3[0][i] = 0;
				drawLogic3[1][i] = 0;
				drawLogic3[2][i] = 0;
				drawLogic3[3][i] = 0;
				drawLogic3[4][i] = 0;
				drawLogic3[5][i] = 0;
				drawLogic3[6][i] = 0;
				drawLogic3[7][i] = 0;
				drawLogic3[8][i] = 0;
				drawLogic3[9][i] = 0;
				drawLogic3[10][i] = 0;
				drawLogic3[11][i] = 0;
			}
			draw_text("PRESS ENTER", -40, 0, 1, 0xffffff);
			draw_text("TO TRY AGAIN", -42.5, -10, 1, 0xffffff);
			draw_text("SCORE:", -32.5, -20, .5, 0xffffff);
			draw_number(player_score, -5, -21.5, .75, 0xffffff);

			if (pressed(BUTTON_ENTER)) {
				hot_button = 0;
				player_lives = 3;
				player_score = 0;
				easySquares = 30;
				mediumSquares = 60;
				hardSquares = 120;
				time = 50000;
				current_gamemode = GM_MENU;

			}

		}
		//Player has won
		else if (easySquares == 0 || mediumSquares == 0 || hardSquares == 0)
		{
			for (int i = 0; i < 10; i++)
			{
				//RESET DRAW LOGIC
				drawLogic[0][i] = 0;
				drawLogic[1][i] = 0;
				drawLogic[2][i] = 0;

				drawLogic2[0][i] = 0;
				drawLogic2[1][i] = 0;
				drawLogic2[2][i] = 0;
				drawLogic2[3][i] = 0;
				drawLogic2[4][i] = 0;
				drawLogic2[5][i] = 0;

				drawLogic3[0][i] = 0;
				drawLogic3[1][i] = 0;
				drawLogic3[2][i] = 0;
				drawLogic3[3][i] = 0;
				drawLogic3[4][i] = 0;
				drawLogic3[5][i] = 0;
				drawLogic3[6][i] = 0;
				drawLogic3[7][i] = 0;
				drawLogic3[8][i] = 0;
				drawLogic3[9][i] = 0;
				drawLogic3[10][i] = 0;
				drawLogic3[11][i] = 0;
			}
			current_gamemode = GM_VICTORY;
			
		}
		else
		{
			time--;
			draw_text("LIVES REMAINING:", 40, 42.5, .3f, 0xFFFFFF);
			draw_number(player_lives, 70, 41.75, .45f, 0xFFFFFF);

			draw_text("SCORE:", -73.5, 42.5, .3f, 0xFFFFFF);
			draw_number(player_score, -55, 41.75, .45f, 0xFFFFFF);

			// Rendering

			// Ball

			draw_rect(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 0xffffff);

			//Player
			draw_rect(player_p, -40, player_half_size_y, player_half_size_x, 0XBD6EFF);

			//Ball Movement
			if (pressed(BUTTON_SPACE)) ball_dp_y = -130;

			//Map
			if (hot_button == 0)
			{
				// Draw Map
				for (int i = 0; i < 3; i++)
				{
					for (int j = 0; j < 10; j++)
					{
						if (drawLogic[i][j] == 0) draw_rect(-67.5 + (15 * j), 32 - (5 * i), 7, 2, colors[i]);
						check_rect_hit(32 - (5 * i), 34 - (5 * i), (-73 + (15 * j)), (-55 + (15 * j)), drawLogic, i, j, easySquares);
					}
				}
			}

			if (hot_button == 1)
			{
				// Draw Map
				for (int i = 0; i < 6; i++)
				{
					for (int j = 0; j < 10; j++)
					{
						if (drawLogic2[i][j] == 0) draw_rect(-67.5 + (15 * j), 32 - (5 * i), 7, 2, colors[i]);
						check_rect_hit(32 - (5 * i), 34 - (5 * i), (-73 + (15 * j)), (-55 + (15 * j)), drawLogic2, i, j, mediumSquares);
					}
				}
			}

			if (hot_button == 2)
			{
				player_lives = 1;
				// Draw Map
				for (int i = 0; i < 12; i++)
				{
					for (int j = 0; j < 10; j++)
					{
						if (drawLogic3[i][j] == 0) draw_rect(-67.5 + (15 * j), 32 - (5 * i), 7, 2, colors[i % 6]);
						check_rect_hit(32 - (5 * i), 34 - (5 * i), (-73 + (15 * j)), (-55 + (15 * j)), drawLogic3, i, j, hardSquares);
					}
				}
			}
		}


	}
	else if (current_gamemode == GM_VICTORY)
	{
		draw_rect(0, 0, arena_half_size_x, arena_half_size_y + 10, 0xc6ca45);
		draw_text("YOU WIN!", -50, 20, 2, 0xffffff);
		draw_text("PRESS ENTER", -40, 0, 1, 0xffffff);
		draw_text("TO PLAY AGAIN", -42.5, -10, 1, 0xffffff);
		draw_text("RAW SCORE:", -47.5, -20, .5, 0xffffff);
		draw_number(player_score, 5, -21.5, .75, 0xffffff);
		draw_text("LIFE BONUS:", -47.5, -26, .5, 0xffffff);
		draw_number((player_lives * 500), 5, -27.5, .75, 0xffffff);
		draw_text("TIME BONUS:", -47.5, -32, .5, 0xffffff);
		draw_number(time / 10, 5, -33.5, .75, 0xffffff);
		draw_text("FINAL SCORE:", -47.5, -38, .5, 0xffffff);
		draw_number(((player_lives * 500) + player_score + (time /10)), 5, -39.5, .75, 0xffffff);
		if (pressed(BUTTON_ENTER)) {
			hot_button = 0;
			player_lives = 3;
			player_score = 0;
			easySquares = 30;
			mediumSquares = 60;
			hardSquares = 120;
			time = 50000;
			current_gamemode = GM_MENU;
		}
	}
	else {
		ball_dp_x = 0;
		ball_p_y = -35;
		ball_p_x = 0;
		ball_dp_y = 0;
		player_p = 0;

		if (pressed(BUTTON_DOWN)) {
			hot_button++;
			if (hot_button == 3) hot_button = 0;
		}
		if (pressed(BUTTON_UP)) {
			hot_button--;
			if (hot_button == -1) hot_button = 2;
		}



		if (pressed(BUTTON_ENTER)) {
			current_gamemode = GM_GAMEPLAY;
		}

		if (hot_button == 0) {
			draw_text("EASY", -20, 0, 1, 0xff0000);
			draw_text("MEDIUM", -20, -10, 1, 0xaaaaaa);
			draw_text("HARD", -20, -20, 1, 0xaaaaaa);
		}
		else if (hot_button == 1) {
			draw_text("EASY", -20, 0, 1, 0xaaaaaa);
			draw_text("MEDIUM", -20, -10, 1, 0xff0000);
			draw_text("HARD", -20, -20, 1, 0xaaaaaa);
		}
		else {
			draw_text("EASY", -20, 0, 1, 0xaaaaaa);
			draw_text("MEDIUM", -20, -10, 1, 0xaaaaaa);
			draw_text("HARD", -20, -20, 1, 0xff0000);
		}

		draw_text("BREAKOUT", -50, 40, 2, 0xffffff);
		draw_text("IN C++", -20, 22, .75, 0xffffff);
		draw_text("CREATED BY LEO FRIEDMAN", -55, 15, .75, 0xffffff);

	}
}
