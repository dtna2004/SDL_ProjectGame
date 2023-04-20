
#include"Common_Function.h"
#include"MainObject.h"
#include"ThreatsObject.h"
#include"ExplosionObject.h"

bool Init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		return false;
	}

	g_screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);

	if (g_screen == NULL){
		return false;
	}
	return true;
}



int main(int arc, char* argv[])
{
	bool is_run_screen = true;
	int bkgn_x = 0;
	bool is_quit = false;
	if (Init() == false){
		return 0;
	}
	g_bkground = SDLCommonFunc::LoadImage("bg4800.png");
	if (g_bkground == NULL)
	{
		return 0;
	}

	//MAKE MAINOBJECT

	MainObject human_object;
	human_object.SetRect(100, 200);
	bool ret = human_object.LoadImg("plane_fly.png");
	if (!ret)
	{
		return 0;
	}

	//Init Explosion Object
	ExplosionObject exp_main;
	ret = exp_main.LoadImg("exp_main.png");
	exp_main.set_clip();
	if (ret == false) return 0;

	//MAKE THREATOBJECT
	ThreatObject* p_threats = new ThreatObject[NUM_THREATS];

	for (int t = 0; t < NUM_THREATS; t++)
	{
		ThreatObject* p_threat = (p_threats + t);
		if (p_threat)
		{
			ret = p_threat->LoadImg("af1.png");
			if (ret == false)
			{
				return 0;
			}

			int rand_y = rand() % 400;
			if (rand_y > SCREEN_HEIGHT - 300)
			{
				rand_y = SCREEN_HEIGHT*0.3;
			}

			p_threat->SetRect(SCREEN_WIDTH + t * 400, rand_y);
			p_threat->set_x_val(3);
			
			
			AmoObject* p_amo = new AmoObject();
			p_threat->InitAmo(p_amo);
		}
	}

	


	while (!is_quit)
	{
		while (SDL_PollEvent(&g_even))
		{
			if (g_even.type == SDL_QUIT)
			{
				is_quit = true;
				break;
			}
			human_object.HandleInputAction(g_even);
		}
		//Apply Background
		bkgn_x -= 1;
		SDLCommonFunc::ApplySurface(g_bkground, g_screen, bkgn_x, 0);
		SDLCommonFunc::ApplySurface(g_bkground, g_screen, bkgn_x + SCREEN_WIDTH, 0);
		if (bkgn_x <= -(WIDTH_BACKGROUND - SCREEN_WIDTH))
		{
			bkgn_x = 0;
		}
		/*if (is_run_screen == true)
		{
			bkgn_x -= 2;
			if (bkgn_x <= -(WIDTH_BACKGROUND - SCREEN_WIDTH))
			{
				is_run_screen = false;
			}
			else 
			{
				SDLCommonFunc::ApplySurface(g_bkground, g_screen, bkgn_x, 0);
			}
		}
		else 
		{
			SDLCommonFunc::ApplySurface(g_bkground, g_screen, bkgn_x, 0);
		}
		*/

		//Implement main object
		human_object.HandleMove();
		human_object.Show(g_screen);
		human_object.MakeAmo(g_screen);


		//Implement Threats Object
		for (int tt = 0; tt < NUM_THREATS; tt++)
		{
			ThreatObject* p_threat = (p_threats + tt);
			if (p_threat)
			{
				p_threat->HandleMove(SCREEN_WIDTH, SCREEN_HEIGHT);
				p_threat->Show(g_screen);
				p_threat->MakeAmo(g_screen, SCREEN_WIDTH, SCREEN_HEIGHT);


			
				//Check Collision human và threats
				bool is_col = SDLCommonFunc::CheckCollision(human_object.GetRect(), p_threat->GetRect());
				if (is_col)
				{
					for (int ex = 0; ex < 4; ex++)
					{
						int x_pos = (human_object.GetRect().x + human_object.GetRect().w*0.5) - EXP_WIDTH*0.5;
						int y_pos = (human_object.GetRect().x + human_object.GetRect().h*0.5) - EXP_HEIGHT*0.5;

						exp_main.SetRect(x_pos, y_pos);
						exp_main.set_frame(ex);
						exp_main.ShowEx(g_screen);
						SDL_Delay(100);
						if (SDL_Flip(g_screen) == -1)
						{
							delete[] p_threats;
							SDLCommonFunc::CleanUp();
							SDL_Quit();
							return 0;
						}
					}

					if (MessageBox(NULL, L"GAME OVER!", L"Thông báo", MB_OK) == IDOK)
					{
						delete[] p_threats;
						SDLCommonFunc::CleanUp();
						SDL_Quit();
						return 0;
					}
				}
				// Check Collision amo of human và threat
				std::vector<AmoObject*> amo_list = human_object.GetAmoList();
				for (int im = 0; im < amo_list.size(); im++)
				{
					AmoObject* p_amo = amo_list.at(im);
					if (p_amo != NULL)
					{
						bool ret_col = SDLCommonFunc::CheckCollision(p_amo->GetRect(), p_threat->GetRect());
						if (ret_col)
						{
							p_threat->Reset(SCREEN_WIDTH + tt*400);
							human_object.RemoveAmo(im);
						}
					}
				}
				//Check Collision human_Object and amo_threats
				std::vector<AmoObject*> kamo_list = p_threats->GetAmoList();
				for (int it = 0; it < kamo_list.size(); it++)
				{
					AmoObject* p_amo = kamo_list.at(it);
					if (p_amo != NULL)
					{
						bool net_col = SDLCommonFunc::CheckCollision(human_object.GetRect(), p_amo->GetRect());
						if (net_col)
						{
							for (int ext = 0; ext < 4; ext++)
							{
								int x_pos = p_amo->GetRect().x - EXP_WIDTH*0.5;
								int y_pos = p_amo->GetRect().y - EXP_HEIGHT*0.5;

								exp_main.set_frame(ext);
								exp_main.SetRect(x_pos, y_pos);
								exp_main.ShowEx(g_screen);
								SDL_Delay(100);
								if (SDL_Flip(g_screen) == -1)
								{
									delete[] p_threats;
									SDLCommonFunc::CleanUp();
									SDL_Quit();
									return 0;
								}
							}
							if (MessageBox(NULL, L"GAME OVER!", L"Thông báo", MB_OK) == IDOK)
							{
								delete[] p_threats;
								SDLCommonFunc::CleanUp();
								SDL_Quit();
								return 0;
							}
						}
					}
				}					
			}
		}

		//Update Screen

		if (SDL_Flip(g_screen) == -1)
			return 0;
	}



	delete[] p_threats;

	SDLCommonFunc::CleanUp();
	SDL_Quit();

	return 0;

}