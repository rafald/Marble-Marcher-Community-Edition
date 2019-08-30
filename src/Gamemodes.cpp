#include "Gamemodes.h"



//Global variables
sf::Vector2i mouse_pos, mouse_prev_pos;
InputState io_state;

bool all_keys[sf::Keyboard::KeyCount] = { 0 };
bool mouse_clicked = false;
bool show_cheats = false;

//Constants
float mouse_sensitivity = 0.005f;
float wheel_sensitivity = 0.2f;
float music_vol = 75.0f;
float target_fps = 60.0f;

GameMode game_mode = MAIN_MENU;

void OpenMainMenu(Scene * scene, Overlays * overlays)
{
	RemoveAllObjects();
	game_mode = MAIN_MENU;

	if (current_music != scene->levels.GetMusic("menu.ogg"))
	{
		if (current_music != nullptr)
			current_music->stop();
		current_music = scene->levels.GetMusic("menu.ogg");
		current_music->play();
	}

	scene->SetExposure(1.0f);
	scene->SetMode(Scene::INTRO);
	sf::Vector2f wsize = default_size;
	sf::Vector2f vsize = default_view.getSize();
	MenuBox mainmenu(1000, vsize.y*0.95f, wsize.x*0.025, wsize.y*0.025f);
	mainmenu.SetBackgroundColor(sf::Color::Transparent);
	//make the menu static
	mainmenu.static_object = true;

	//TITLE
	Text ttl("Marble\nMarcher", LOCAL("default"), 120, sf::Color::White);
	ttl.SetBorderColor(sf::Color::Black);
	ttl.SetBorderWidth(4);
	mainmenu.AddObject(&ttl, Object::Allign::LEFT);

	Box margin1(800, 5);
	margin1.SetBackgroundColor(sf::Color::Transparent);
	mainmenu.AddObject(&margin1, Object::Allign::LEFT);

	Text CE("Community Edition", LOCAL("default"), 60, sf::Color::White);
	CE.SetBorderColor(sf::Color::Black);
	CE.SetBorderWidth(4);
	mainmenu.AddObject(&CE, Object::Allign::LEFT);

	Box margin(800, 80);
	margin.SetBackgroundColor(sf::Color::Transparent);
	mainmenu.AddObject(&margin, Object::Allign::LEFT);

	//PLAY
	Box playbtn(600, 50);
	Text button1(LOCAL["Play"], LOCAL("default"), 40, sf::Color::White);
	button1.SetBorderColor(sf::Color::Black);
	playbtn.hoverstate.color_main = sf::Color(200, 40, 0, 255);
	playbtn.SetCallbackFunction([scene, overlays](sf::RenderWindow * window, InputState & state)
	{
		PlayNewGame(scene, window, 0);
		overlays->sound_click.play();
	}, true);
	playbtn.AddObject(&button1, Object::Allign::CENTER);
	mainmenu.AddObject(&playbtn, Object::Allign::LEFT);

	//LEVELS
	Box lvlsbtn(600, 50);
	Text button2(LOCAL["Levels"], LOCAL("default"), 40, sf::Color::White);
	button2.SetBorderColor(sf::Color::Black);
	lvlsbtn.hoverstate.color_main = sf::Color(200, 40, 0, 255);
	lvlsbtn.SetCallbackFunction([scene, overlays](sf::RenderWindow * window, InputState & state)
	{
		OpenLevelMenu(scene, overlays);
		overlays->sound_click.play();
	}, true);
	lvlsbtn.AddObject(&button2, Object::Allign::CENTER);
	mainmenu.AddObject(&lvlsbtn, Object::Allign::LEFT);

	//Settings
	Box sttbtn(600, 50);
	Text buttonstt(LOCAL["Settings"], LOCAL("default"), 40, sf::Color::White);
	buttonstt.SetBorderColor(sf::Color::Black);
	sttbtn.hoverstate.color_main = sf::Color(200, 40, 0, 255);
	sttbtn.SetCallbackFunction([scene, overlays](sf::RenderWindow * window, InputState & state)
	{
		overlays->TWBAR_ENABLED = !overlays->TWBAR_ENABLED;
		overlays->sound_click.play();
	}, true);
	sttbtn.AddObject(&buttonstt, Object::Allign::CENTER);
	mainmenu.AddObject(&sttbtn, Object::Allign::LEFT);
	
	//Controls
	Box cntrlbtn(600, 50);
	Text button3(LOCAL["Controls"], LOCAL("default"), 40, sf::Color::White);
	button3.SetBorderColor(sf::Color::Black);
	cntrlbtn.hoverstate.color_main = sf::Color(200, 40, 0, 255);
	cntrlbtn.SetCallbackFunction([scene, overlays](sf::RenderWindow * window, InputState & state)
	{
		OpenControlMenu(scene, overlays);
		overlays->sound_click.play();
	}, true);
	cntrlbtn.AddObject(&button3, Object::Allign::CENTER);
	mainmenu.AddObject(&cntrlbtn, Object::Allign::LEFT);

	//Screen Saver
	Box ssbtn(600, 50);
	Text button4(LOCAL["Screen_Saver"], LOCAL("default"), 40, sf::Color::White);
	button4.SetBorderColor(sf::Color::Black);
	ssbtn.hoverstate.color_main = sf::Color(200, 40, 0, 255);
	ssbtn.SetCallbackFunction([scene, overlays](sf::RenderWindow * window, InputState & state)
	{
		OpenScreenSaver(scene, overlays);
		overlays->sound_click.play();
	}, true);
	ssbtn.AddObject(&button4, Object::Allign::CENTER);
	mainmenu.AddObject(&ssbtn, Object::Allign::LEFT);

	//Exit
	Box exitbtn(600, 50);
	Text button5(LOCAL["Exit"], LOCAL("default"), 40, sf::Color::White);
	button5.SetBorderColor(sf::Color::Black);
	exitbtn.hoverstate.color_main = sf::Color(200, 40, 0, 255);
	exitbtn.SetCallbackFunction([scene, overlays](sf::RenderWindow * window, InputState & state)
	{
		overlays->sound_click.play();
		window->close();
	}, true);
	exitbtn.AddObject(&button5, Object::Allign::CENTER);
	mainmenu.AddObject(&exitbtn, Object::Allign::LEFT);

	Text about(LOCAL["About"], LOCAL("mono"), 30, sf::Color::White);
	about.SetBorderColor(sf::Color::Black);
	about.SetBorderWidth(3);
	mainmenu.AddObject(&about, Object::Allign::LEFT);
	

	AddGlobalObject(mainmenu);
}

void OpenEditor(Scene * scene, Overlays * overlays, int level)
{
	RemoveAllObjects();
	//go to level editor
	game_mode = LEVEL_EDITOR;
	scene->SetExposure(1.0f);
	overlays->TWBAR_ENABLED = true;
	TwDefine("LevelEditor visible=true position='20 20'");
	TwDefine("FractalEditor visible=true position='20 500'");
	TwDefine("Settings iconified=true");
	TwDefine("Statistics iconified=true");
	scene->StartLevelEditor(level);
}

void PlayLevel(Scene * scene, sf::RenderWindow * window, int level)
{
	RemoveAllObjects();
	//play level
	game_mode = PLAYING;
	scene->StartSingle(level);
	LockMouse(*window);
}


void OpenControlMenu(Scene * scene, Overlays * overlays)
{
	RemoveAllObjects();
	game_mode = CONTROLS;
}




void OpenPauseMenu(Scene * scene, Overlays * overlays)
{
	RemoveAllObjects();
	scene->SetExposure(1.0f);
	sf::Vector2f wsize = default_size;
	sf::Vector2f vsize = default_view.getSize();
	MenuBox pausemenu(625, 510, wsize.x*0.025, wsize.y*0.025f);
	pausemenu.SetBackgroundColor(sf::Color(32, 32, 32, 200));
	
	//make the menu static
	pausemenu.static_object = true;

	//TITLE
	Text ttl(LOCAL["Paused"], LOCAL("default"), 120, sf::Color::White);
	ttl.SetBorderColor(sf::Color::Black);
	ttl.SetBorderWidth(4);
	pausemenu.AddObject(&ttl, Object::Allign::LEFT);

	Box margin1(600, 0);
	margin1.SetBackgroundColor(sf::Color::Transparent);
	pausemenu.AddObject(&margin1, Object::Allign::LEFT);

	Text CE1(scene->level_copy.txt, LOCAL("default"), 40, sf::Color::White);
	CE1.SetBorderColor(sf::Color::Black);
	CE1.SetBorderWidth(0);
	pausemenu.AddObject(&CE1, Object::Allign::LEFT);

	pausemenu.AddObject(&margin1, Object::Allign::LEFT);

	Text CE2(scene->level_copy.desc, LOCAL("default"), 20, sf::Color::White);
	CE2.SetBorderColor(sf::Color::Black);
	CE2.SetBorderWidth(0);
	pausemenu.AddObject(&CE2, Object::Allign::LEFT);

	Box margin(600, 20);
	margin.SetBackgroundColor(sf::Color::Transparent);
	pausemenu.AddObject(&margin, Object::Allign::LEFT);

	//PLAY
	Box resumebtn(600, 50);
	Text button1(LOCAL["Resume"], LOCAL("default"), 40, sf::Color::White);
	button1.SetBorderColor(sf::Color::Black);
	button1.SetBorderWidth(2);
	resumebtn.SetBackgroundColor(sf::Color(200, 200, 200, 200));
	resumebtn.hoverstate.color_main = sf::Color(200, 40, 0, 255);
	resumebtn.SetCallbackFunction([scene, overlays](sf::RenderWindow * window, InputState & state)
	{
		RemoveAllObjects();
		game_mode = PLAYING;
		scene->GetCurMusic().setVolume(GetVol());
		scene->SetExposure(1.0f);
		LockMouse(*window);
		overlays->sound_click.play();
	}, true);
	resumebtn.AddObject(&button1, Object::Allign::CENTER);
	pausemenu.AddObject(&resumebtn, Object::Allign::LEFT);

	//LEVELS
	Box rstbtn(600, 50);
	Text button2(LOCAL["Restart"], LOCAL("default"), 40, sf::Color::White);
	button2.SetBorderColor(sf::Color::Black);
	button2.SetBorderWidth(2);
	rstbtn.SetBackgroundColor(sf::Color(200, 200, 200, 200));
	rstbtn.hoverstate.color_main = sf::Color(200, 40, 0, 255);
	rstbtn.SetCallbackFunction([scene, overlays](sf::RenderWindow * window, InputState & state)
	{
		RemoveAllObjects();
		game_mode = PLAYING;
		scene->ResetLevel();
		scene->GetCurMusic().setVolume(GetVol());
		scene->SetExposure(1.0f);
		LockMouse(*window);
		overlays->sound_click.play();
	}, true);
	rstbtn.AddObject(&button2, Object::Allign::CENTER);
	pausemenu.AddObject(&rstbtn, Object::Allign::LEFT);

	//Settings
	Box sttbtn(600, 50);
	Text buttonstt(LOCAL["Settings"], LOCAL("default"), 40, sf::Color::White);
	buttonstt.SetBorderColor(sf::Color::Black);
	buttonstt.SetBorderWidth(2);
	sttbtn.SetBackgroundColor(sf::Color(200, 200, 200, 200));
	sttbtn.hoverstate.color_main = sf::Color(200, 40, 0, 255);
	sttbtn.SetCallbackFunction([scene, overlays](sf::RenderWindow * window, InputState & state)
	{
		overlays->TWBAR_ENABLED = !overlays->TWBAR_ENABLED;
		overlays->sound_click.play();
	}, true);
	sttbtn.AddObject(&buttonstt, Object::Allign::CENTER);
	pausemenu.AddObject(&sttbtn, Object::Allign::LEFT);

	//Exit
	Box exitbtn(600, 50);
	Text button5(LOCAL["Quit"], LOCAL("default"), 40, sf::Color::White);
	button5.SetBorderColor(sf::Color::Black);
	button5.SetBorderWidth(2);
	exitbtn.SetBackgroundColor(sf::Color(200, 200, 200, 200));
	exitbtn.hoverstate.color_main = sf::Color(200, 40, 0, 255);
	exitbtn.SetCallbackFunction([scene, overlays](sf::RenderWindow * window, InputState & state)
	{
		RemoveAllObjects();
		if (scene->IsSinglePlay())
		{
			OpenLevelMenu(scene, overlays);
		}
		else
		{
			OpenMainMenu(scene, overlays);
		}
		scene->SetMode(Scene::INTRO);
		scene->StopAllMusic();
	}, true);
	exitbtn.AddObject(&button5, Object::Allign::CENTER);
	pausemenu.AddObject(&exitbtn, Object::Allign::LEFT);

	AddGlobalObject(pausemenu);
}

void PauseGame(sf::RenderWindow& window, Overlays * overlays, Scene * scene) {
	game_mode = PAUSED;
	scene->GetCurMusic().setVolume(GetVol());
	UnlockMouse(window);
	OpenPauseMenu(scene, overlays);
	scene->SetExposure(0.5f);
}

void OpenScreenSaver(Scene * scene, Overlays * overlays)
{
	RemoveAllObjects();
	game_mode = SCREEN_SAVER;
	scene->SetMode(Scene::SCREEN_SAVER);
}

void PlayNewGame(Scene * scene, sf::RenderWindow * window, int level)
{
	RemoveAllObjects();
	game_mode = PLAYING;
	scene->StartNewGame();
//	scene->GetCurMusic().setVolume(GetVol());
//	scene->GetCurMusic().play();
	LockMouse(*window);
}

void OpenTestWindow()
{
	Window test(200, 200, 500, 500, sf::Color(0, 0, 0, 128), LOCAL["Window"], LOCAL("default"));
	Text button(LOCAL["Button"], LOCAL("default"), 30, sf::Color::White);
	Box sbox(0, 0, 420, 200, sf::Color(128, 128, 128, 240));
	Box sbox2(0, 0, 240, 40, sf::Color(0, 64, 128, 240));
	Box sbox3(0, 0, 30, 30, sf::Color(0, 64, 128, 240));

	sbox2.hoverstate.color_main = sf::Color(230, 40, 20, 200);
	sbox2.AddObject(&button, Box::CENTER);
	button.hoverstate.font_size = 40;
	test.Add(&sbox, Box::CENTER);
	test.Add(&sbox2, Box::CENTER);
	test.Add(&sbox, Box::CENTER);
	test.Add(&sbox2, Box::CENTER);
	test.Add(&sbox, Box::CENTER);
	test.Add(&sbox2, Box::CENTER);
	
	AddGlobalObject(test);
}

void OpenLevelMenu(Scene* scene, Overlays* overlays)
{
	RemoveAllObjects();
	sf::Vector2f wsize = default_size;
	sf::Vector2f vsize = default_view.getSize();
	MenuBox levels(wsize.x*0.95f, vsize.y*0.95f, (vsize.x - wsize.x*0.95f)/2, vsize.y*0.025f);
	levels.SetBackgroundColor(sf::Color(32,32,32,160));
	//make the menu static
	levels.static_object = true;

	scene->SetExposure(0.7f);
	scene->SetMode(Scene::INTRO);
	game_mode = LEVELS;

	Box lvlmargin(50, 4);
	lvlmargin.SetBackgroundColor(sf::Color::Transparent);

	std::map<int, std::string> names = scene->levels.getLevelNames();
	std::map<int, std::string> desc = scene->levels.getLevelDesc();
	std::vector<int> ids = scene->levels.getLevelIds();
	std::map<int, Score> scores = scene->levels.getLevelScores();
	Text lvl(LOCAL["Levels"], LOCAL("default"), 60, sf::Color::White);
	levels.AddObject(&lvl, Object::Allign::CENTER);
	levels.AddObject(&lvlmargin, Object::Allign::CENTER);
	Box Bk2Menu(600, 50);
	Bk2Menu.SetBackgroundColor(sf::Color(128, 128, 128, 128));
	Text button(LOCAL["Back2Main"], LOCAL("default"), 40, sf::Color::White);
	Bk2Menu.hoverstate.color_main = sf::Color(200, 40, 0, 255);
	Bk2Menu.SetCallbackFunction([scene, overlays](sf::RenderWindow * window, InputState & state)
	{
		OpenMainMenu(scene, overlays);
		overlays->sound_click.play();
	});
	Bk2Menu.AddObject(&button, Object::Allign::CENTER);
	levels.AddObject(&Bk2Menu, Object::Allign::LEFT);

	Box Newlvl(600, 50);
	Newlvl.SetBackgroundColor(sf::Color(128, 128, 128, 128));
	Text newlvl(LOCAL["CreateNewLvl"], LOCAL("default"), 40, sf::Color::White);
	Newlvl.hoverstate.color_main = sf::Color(200, 40, 0, 255);
	Newlvl.SetCallbackFunction([scene, overlays](sf::RenderWindow * window, InputState & state)
	{
		OpenEditor(scene, overlays, -1);
		overlays->sound_click.play();
	});
	Newlvl.AddObject(&newlvl, Object::Allign::CENTER);
	levels.AddObject(&Newlvl, Object::Allign::LEFT);
	
	sf::Image edit; edit.loadFromFile(edit_png);
	sf::Texture edittxt; edittxt.loadFromImage(edit);
	edittxt.setSmooth(true);

	sf::Image remove; remove.loadFromFile(delete_png);
	sf::Texture removetxt; removetxt.loadFromImage(remove);
	removetxt.setSmooth(true);

	for (int i = 0; i < scene->levels.GetLevelNum(); i++)
	{
		Box lvlbtton(wsize.x*0.95f - 60, 65);
		lvlbtton.SetBackgroundColor(sf::Color(128, 128, 128, 128));
		lvlbtton.hoverstate.border_thickness = 3;

		Box lvltext(500, 65);
		lvltext.SetBackgroundColor(sf::Color::Transparent);
		Box lvltitle(500, 40);
		lvltitle.SetBackgroundColor(sf::Color::Transparent);
		Text lvlname(utf8_to_wstring(names[ids[i]]), LOCAL("default"), 35, sf::Color::White);
		Text lvldescr(utf8_to_wstring(desc[ids[i]]), LOCAL("default"), 18, sf::Color::White);
		lvlname.hoverstate.color_main = sf::Color(255, 0, 0, 255);
		lvlname.SetCallbackFunction([scene, overlays, selected = ids[i]](sf::RenderWindow * window, InputState & state)
		{
			PlayLevel(scene, window, selected);
			overlays->sound_click.play();
		});
		lvltitle.AddObject(&lvlname, Object::Allign::LEFT);
		lvltext.AddObject(&lvltitle, Object::Allign::LEFT);
	
		lvltext.AddObject(&lvlmargin, Object::Allign::CENTER);
		lvltext.AddObject(&lvldescr, Object::Allign::LEFT);
		lvlbtton.AddObject(&lvltext, Object::Allign::LEFT);

		Box lvlscore(500, 40);
		lvlscore.SetBackgroundColor(sf::Color::Transparent);
		std::string score_text = "--:--:--";
		if (scores[ids[i]].best_time != 0)
		{
			float time = scores[ids[i]].best_time;
			float minutes = floor(time / 60.f);
			float seconds = floor(time) - minutes*60;
			float mili = floor(time*100) - seconds*100 - minutes*6000;
			//convrt mili to frames
			score_text = num2str(minutes) + ":" + num2str(seconds) + ":" + num2str(floor(mili*0.6f));
		}
		Text lvlscorev(score_text, LOCAL("default"), 35, sf::Color::White);
		lvlscorev.SetBackgroundColor(sf::Color::Green);
		lvlscore.AddObject(&lvlscorev, Object::Allign::CENTER);
		lvlbtton.AddObject(&lvlscore, Object::Allign::LEFT);

		Box lvlavg(500, 40);
		lvlavg.SetBackgroundColor(sf::Color::Transparent);
		score_text = "--:--:--";
		if (scores[ids[i]].best_time != 0)
		{
			float time = scores[ids[i]].all_time/scores[ids[i]].played_num;
			float minutes = floor(time / 60.f);
			float seconds = floor(time) - minutes * 60;
			float mili = floor(time * 100) - seconds * 100 - minutes * 6000;
			score_text = num2str(minutes) + ":" + num2str(seconds) + ":" + num2str(floor(mili*0.6f));
		}
		Text lvlavgtxt(score_text, LOCAL("default"), 35, sf::Color::White);
		lvlavgtxt.SetBackgroundColor(sf::Color::White);
		lvlavg.AddObject(&lvlavgtxt, Object::Allign::CENTER);
		lvlbtton.AddObject(&lvlavg, Object::Allign::LEFT);

		Box buttons(120, 60);
		buttons.SetBackgroundColor(sf::Color::Transparent);
		Box bedit(60, 60);
		bedit.defaultstate.color_main = sf::Color(255, 255, 255, 255);
		bedit.hoverstate.color_main = sf::Color(0, 255, 0, 255);
		bedit.SetBackground(edittxt);
		bedit.SetCallbackFunction([scene, overlays, id = ids[i]](sf::RenderWindow * window, InputState & state)
		{
			OpenEditor(scene, overlays, id);
			overlays->sound_click.play();
		}, true);

		Box bremove(60, 60);
		bremove.defaultstate.color_main = sf::Color(255, 255, 255, 255);
		bremove.hoverstate.color_main = sf::Color(255, 0, 0, 255);
		bremove.SetBackground(removetxt);

		bremove.SetCallbackFunction([scene, overlays, id = ids[i]](sf::RenderWindow * window, InputState & state)
		{
			ConfirmLevelDeletion(id, scene, overlays);
			overlays->sound_click.play();
		}, true);

		buttons.AddObject(&bremove, Object::Allign::RIGHT);
		buttons.AddObject(&bedit, Object::Allign::RIGHT);
		lvlbtton.AddObject(&buttons, Object::Allign::RIGHT);
		levels.AddObject(&lvlbtton, Object::Allign::LEFT);
	}

	AddGlobalObject(levels);
}

void ConfirmLevelDeletion(int lvl, Scene* scene, Overlays* overlays)
{
	sf::Vector2f wsize = default_size;
	Window confirm(wsize.x*0.4f, wsize.y*0.4f, 500, 215, sf::Color(0, 0, 0, 128), LOCAL["You_sure"], LOCAL("default"));
	Text button1(LOCAL["Yes"], LOCAL("default"), 30, sf::Color::White);
	Text button2(LOCAL["No"], LOCAL("default"), 30, sf::Color::White);
	Text text(LOCAL["You_sure"], LOCAL("default"), 30, sf::Color::White);
	 
	Box but1(0, 0, 240, 40, sf::Color(0, 64, 128, 240));
	Box but2(0, 0, 240, 40, sf::Color(0, 64, 128, 240));

	but1.hoverstate.color_main = sf::Color(230, 40, 20, 200);
	but2.hoverstate.color_main = sf::Color(40, 230, 20, 200);
	but1.AddObject(&button1, Box::CENTER);
	but2.AddObject(&button2, Box::CENTER);

	confirm.Add(&text, Box::CENTER);
	confirm.Add(&but1, Box::RIGHT);
	confirm.Add(&but2, Box::RIGHT);

	int id = AddGlobalObject(confirm);

	get_glob_obj(id).objects[1].get()->objects[0].get()->objects[1].get()->SetCallbackFunction([scene, overlays, id, lvl](sf::RenderWindow * window, InputState & state)
	{
		//remove lvl
		scene->levels.DeleteLevel(lvl);
		overlays->sound_click.play();
		OpenLevelMenu(scene, overlays);
	});

	get_glob_obj(id).objects[1].get()->objects[0].get()->objects[2].get()->SetCallbackFunction([scene, overlays, id](sf::RenderWindow * window, InputState & state)
	{
		Add2DeleteQueue(id);
		overlays->sound_click.play();
	});
}

void ConfirmEditorExit(Scene* scene, Overlays* overlays)
{
	sf::Vector2f wsize = default_size;
	Window confirm(wsize.x*0.4f, wsize.y*0.4f, 500, 215, sf::Color(0, 0, 0, 128), LOCAL["You_sure"], LOCAL("default"));
	Text button1(LOCAL["Yes"], LOCAL("default"), 30, sf::Color::White);
	Text button2(LOCAL["No"], LOCAL("default"), 30, sf::Color::White);
	Text text(LOCAL["You_sure"], LOCAL("default"), 30, sf::Color::White);

	Box but1(0, 0, 240, 40, sf::Color(0, 64, 128, 240));
	Box but2(0, 0, 240, 40, sf::Color(0, 64, 128, 240));

	but1.hoverstate.color_main = sf::Color(230, 40, 20, 200);
	but2.hoverstate.color_main = sf::Color(40, 230, 20, 200);
	but1.AddObject(&button1, Box::CENTER);
	but2.AddObject(&button2, Box::CENTER);

	confirm.Add(&text, Box::CENTER);
	confirm.Add(&but1, Box::RIGHT);
	confirm.Add(&but2, Box::RIGHT);

	int id = AddGlobalObject(confirm);

	get_glob_obj(id).objects[1].get()->objects[0].get()->objects[1].get()->SetCallbackFunction([scene, overlays, id](sf::RenderWindow * window, InputState & state)
	{
		OpenLevelMenu(scene, overlays);
		scene->ExitEditor();
		scene->StopAllMusic();
		overlays->TWBAR_ENABLED = false;
		TwDefine("LevelEditor visible=false");
		TwDefine("FractalEditor visible=false");
		overlays->sound_click.play();
	});

	get_glob_obj(id).objects[1].get()->objects[0].get()->objects[2].get()->SetCallbackFunction([scene, overlays, id](sf::RenderWindow * window, InputState & state)
	{
		Add2DeleteQueue(id);
		overlays->sound_click.play();
	});
}


float GetVol() {
	if (game_mode == PAUSED) {
		return music_vol / 4;
	}
	else {
		return music_vol;
	}
}

void LockMouse(sf::RenderWindow& window) {
	window.setMouseCursorVisible(false);
	const sf::Vector2u size = window.getSize();
	mouse_pos = sf::Vector2i(size.x / 2, size.y / 2);
	sf::Mouse::setPosition(mouse_pos);
}
void UnlockMouse(sf::RenderWindow& window) {
	window.setMouseCursorVisible(true);
}


int DirExists(const char *path) {
	struct stat info;
	if (stat(path, &info) != 0) {
		return 0;
	}
	else if (info.st_mode & S_IFDIR) {
		return 1;
	}
	return 0;
}

void FirstStart(Overlays* overlays)
{
	TwDefine("First_launch visible=true");
	TwDefine("Statistics visible=false");
	TwDefine("Settings visible=false");
	game_mode = FIRST_START;
	overlays->TWBAR_ENABLED = true;
}


//ANTTWEAKBAR

Scene *scene_ptr;
Overlays *overlays_ptr;
Renderer *renderer_ptr;
sf::Shader *shader_ptr;
sf::RenderTexture *renderTexture;
sf::RenderTexture *screenshotTexture;
sf::Texture *main_txt;
sf::Texture *screenshot_txt;
sf::RectangleShape *rectmain;
sf::RectangleShape *rectscr;


void SetPointers(sf::RenderTexture *render, sf::RenderTexture *screenshot, sf::Texture *main, sf::Texture *screensht, sf::RectangleShape *rmain, sf::RectangleShape *rscr, sf::Shader *shader)
{
	renderTexture = render;
	screenshotTexture = screenshot;
	shader_ptr = shader;
	main_txt = main;
	screenshot_txt = screensht;
	rectmain = rmain;
	rectscr = rscr;
}

void InitializeRendering(std::string config)
{
	scene_ptr->SetResolution(*shader_ptr, SETTINGS.stg.rendering_resolution.x, SETTINGS.stg.rendering_resolution.y);
	renderer_ptr->Initialize(SETTINGS.stg.rendering_resolution.x, SETTINGS.stg.rendering_resolution.y, renderer_ptr->GetConfigFolder() + "/" + config);
	renderer_ptr->variables["MRRM_scale"] = SETTINGS.stg.MRRM_scale;
	renderer_ptr->variables["shadow_scale"] = SETTINGS.stg.shadow_resolution;
	renderer_ptr->variables["bloom_scale"] = SETTINGS.stg.bloom_resolution;
	renderer_ptr->camera.bloomintensity = SETTINGS.stg.bloom_intensity;
	renderer_ptr->camera.bloomradius = SETTINGS.stg.bloom_intensity;
	renderer_ptr->camera.bloomtreshold = SETTINGS.stg.bloom_intensity;
	renderer_ptr->camera.SetFOV(SETTINGS.stg.FOV);

	//GL settings
	sf::ContextSettings settings;
	settings.majorVersion = 4;
	settings.minorVersion = 3;

	renderTexture->clear();
	renderTexture->create(SETTINGS.stg.rendering_resolution.x, SETTINGS.stg.rendering_resolution.y, settings);
	main_txt->create(SETTINGS.stg.rendering_resolution.x, SETTINGS.stg.rendering_resolution.y);
	renderTexture->setSmooth(false);
	renderer_ptr->SetOutputTexture(*main_txt);

	screenshotTexture->clear();
	screenshotTexture->create(SETTINGS.stg.screenshot_resolution.x, SETTINGS.stg.screenshot_resolution.y, settings);
	screenshot_txt->create(SETTINGS.stg.screenshot_resolution.x, SETTINGS.stg.screenshot_resolution.y);
	screenshotTexture->setSmooth(false);

	const sf::Glsl::Vec2 window_res((float)SETTINGS.stg.rendering_resolution.x, (float)SETTINGS.stg.rendering_resolution.y);
	const sf::Glsl::Vec2 sres_res((float)SETTINGS.stg.screenshot_resolution.x, (float)SETTINGS.stg.screenshot_resolution.y);
	//Create screen rectangle
	rectmain->setSize(window_res);
	rectscr->setSize(sres_res);
}

int music_id = 0;
bool music_play = false;

void TW_CALL MarbleSet(void *data)
{
	scene_ptr->cur_ed_mode = Scene::EditorMode::PLACE_MARBLE;
}

void TW_CALL FlagSet(void *data)
{
	scene_ptr->cur_ed_mode = Scene::EditorMode::PLACE_FLAG;
}

void TW_CALL PlayMusic(void *data)
{
	scene_ptr->levels.StopAllMusic();
	music_play = !music_play;
	if (music_play)
	{
		scene_ptr->levels.GetMusicByID(music_id)->play();
	}
}

void TW_CALL SaveLevel(void *data)
{
	Level* copy = &scene_ptr->level_copy;
	int lvlid = scene_ptr->GetLevel();

	std::vector<std::string> music_list = scene_ptr->levels.GetMusicNames();
	std::vector<int> lvlnum = scene_ptr->levels.getLevelIds();
	copy->use_music = music_list[music_id];
	bool same_level = scene_ptr->original_level_name == copy->txt;
	if (lvlid < 0 || !same_level)
		lvlid = time(NULL);
	copy->level_id = lvlid;
	copy->SaveToFile(std::string(level_folder) + "/" + ConvertSpaces2_(copy->txt) + ".lvl", lvlid, copy->link_level);
	scene_ptr->levels.ReloadLevels();
	if (!(scene_ptr->GetLevel() >= 0 && same_level))
	{
		scene_ptr->WriteLVL(lvlid);
		scene_ptr->original_level_name = copy->txt;
	}
}


void TW_CALL CopyStdStringToClient(std::string& destinationClientString, const std::string& sourceLibraryString)
{
	// Copy the content of souceString handled by the AntTweakBar library to destinationClientString handled by your application
	destinationClientString = sourceLibraryString;
}

int render_resolution = 3;
int screenshot_resolution = 10;
int language_choise = 0;
int shader_config = 0;

sf::Vector2i getResolution(int i)
{
	switch (i)
	{
	case 0:
		return sf::Vector2i(320, 240);
	case 1:
		return sf::Vector2i(480, 320);
	case 2:
		return sf::Vector2i(640, 480);
	case 3:
		return sf::Vector2i(800, 480);
	case 4:
		return sf::Vector2i(960, 540);
	case 5:
		return sf::Vector2i(1136, 640);
	case 6:
		return sf::Vector2i(1280, 720);
	case 7:
		return sf::Vector2i(1600, 900);
	case 8:
		return sf::Vector2i(1920, 1080);
	case 9:
		return sf::Vector2i(2048, 1152);
	case 10:
		return sf::Vector2i(2560, 1440);
	case 11:
		return sf::Vector2i(3840, 2160);
	case 12:
		return sf::Vector2i(7680, 4320);
	case 13:
		return sf::Vector2i(10240, 4320);
	}
}

void TW_CALL InitialOK(void *data)
{
	std::vector<std::string> langs = LOCAL.GetLanguages();
	LOCAL.SetLanguage(langs[language_choise]);
	SETTINGS.stg.rendering_resolution = getResolution(render_resolution);
	SETTINGS.stg.screenshot_resolution = getResolution(screenshot_resolution);
	TwDefine("First_launch visible=false");
	TwDefine("Statistics visible=true");
	TwDefine("Settings visible=true");
	overlays_ptr->TWBAR_ENABLED = false;
	OpenMainMenu(scene_ptr, overlays_ptr);
}

void TW_CALL ApplySettings(void *data)
{
	std::vector<std::string> langs = LOCAL.GetLanguages();
	LOCAL.SetLanguage(langs[language_choise]);
	SETTINGS.stg.rendering_resolution = getResolution(render_resolution);
	SETTINGS.stg.screenshot_resolution = getResolution(screenshot_resolution);

	std::vector<std::string> configs = renderer_ptr->GetConfigurationsList();

	InitializeRendering(configs[shader_config]);

	if(current_music != nullptr)
		current_music->setVolume(SETTINGS.stg.music_volume);
}



void InitializeATBWindows(Scene* scene, Overlays* overlays, Renderer* rd, float* fps, bool *vsync, float *mouse_sensitivity, float *wheel_sensitivity, float *music_vol, float *target_fps)
{
	scene_ptr = scene;
	overlays_ptr = overlays;
	renderer_ptr = rd;

	overlays_ptr->stats = TwNewBar("Statistics");
	TwDefine(" GLOBAL help='Marble Marcher: Community Edition. Use F5 to take screenshots. F4 to open or close settings windows.' ");

	std::map<int, std::string> level_list = scene->levels.getLevelNames();
	TwEnumVal *level_enums = new TwEnumVal[level_list.size() + 1];
	TwEnumVal enumval;
	enumval.Label = "None";
	enumval.Value = -1;
	level_enums[0] = enumval;
	int i = 0;
	for (auto &name : level_list)
	{
		enumval.Label = name.second.c_str();
		enumval.Value = i;
		level_enums[i + 1] = enumval;
		i++;
	}

	TwType Levels = TwDefineEnum("levels", level_enums, level_list.size() + 1);

	TwEnumVal resolutions[] = { { 0, "320x240"  },
								{ 1,  "480x320" },
								{ 2,  "640x480" },
								{ 3,  "800x480" },
								{ 4,  "960x540" },
								{ 5,  "1136x640" },
								{ 6,  "1280x720" },
								{ 7,  "1600x900" },
								{ 8,  "1920x1080" },
								{ 9,  "2048x1152" },
								{ 10,  "2560x1440" },
								{ 11,  "3840x2160" },
								{ 12,  "7680x4320" },
								{ 13,  "10240x4320" } };

	TwType Resolutions = TwDefineEnum("Resolutions", resolutions, 14);

	std::vector<std::string> music_list = scene->levels.GetMusicNames();
	TwEnumVal *music_enums = new TwEnumVal[music_list.size()];
	for (int i = 0; i < music_list.size(); i++)
	{
		TwEnumVal enumval;
		enumval.Label = music_list[i].c_str();
		enumval.Value = i;
		music_enums[i] = enumval;
	}

	TwType Level_music = TwDefineEnum("Level music", music_enums, music_list.size());

	std::vector<std::string> langs = LOCAL.GetLanguages();
	TwEnumVal *language_enums = new TwEnumVal[langs.size()];

	for (int j = 0; j < langs.size(); j++)
	{
		enumval.Label = langs[j].c_str();
		enumval.Value = j;
		language_enums[j] = enumval;
	}

	TwType Languages = TwDefineEnum("Languages", language_enums, langs.size());

	std::vector<std::string> configs = rd->GetConfigurationsList();
	TwEnumVal *config_enums = new TwEnumVal[configs.size()];

	for (int j = 0; j < configs.size(); j++)
	{
		enumval.Label = configs[j].c_str();
		enumval.Value = j;
		config_enums[j] = enumval;
	}

	TwType Configurations = TwDefineEnum("Configurations", config_enums, configs.size());

	// Change bar position
	int barPos[2] = { 16, 60 };
	TwSetParam(overlays_ptr->stats, NULL, "position", TW_PARAM_INT32, 2, &barPos);
	TwAddVarRO(overlays_ptr->stats, "FPS", TW_TYPE_FLOAT, fps, " label='FPS' ");
	TwAddVarRO(overlays_ptr->stats, "Marble velocity", TW_TYPE_DIR3F, scene->marble_vel.data(), " ");
	TwAddVarRO(overlays_ptr->stats, "Marble position", TW_TYPE_DIR3F, scene->marble_pos.data(), " ");

	overlays_ptr->settings = TwNewBar("Settings");

	TwAddVarRW(overlays_ptr->settings, "Rendering resolution", Resolutions, &render_resolution, "group='Rendering settings'");
	TwAddVarRW(overlays_ptr->settings, "Screenshot resolution", Resolutions, &screenshot_resolution, "group='Rendering settings'");
	TwAddVarRW(overlays_ptr->settings, "Shader configuration", Configurations, &shader_config, "group='Rendering settings'");
	TwAddVarRW(overlays_ptr->settings, "MRRM scaling", TW_TYPE_INT32, &SETTINGS.stg.MRRM_scale, "min=2 max=8 group='Rendering settings'");
	TwAddVarRW(overlays_ptr->settings, "Shadow downscaling", TW_TYPE_INT32, &SETTINGS.stg.shadow_resolution, "min=1 max=8 group='Rendering settings'");
	TwAddVarRW(overlays_ptr->settings, "Bloom downscaling", TW_TYPE_INT32, &SETTINGS.stg.bloom_resolution, "min=1 max=8 group='Rendering settings'");

	
	TwAddVarRW(overlays_ptr->settings, "FOV", TW_TYPE_FLOAT, &SETTINGS.stg.FOV, "min=30 step=1 max=180 group='Graphics settings'");
	TwAddVarRW(overlays_ptr->settings, "VSYNC", TW_TYPE_BOOLCPP, vsync, "group='Graphics settings'");
	TwAddVarRW(overlays_ptr->settings, "Shadows", TW_TYPE_BOOLCPP, &SETTINGS.stg.shadows, "group='Graphics settings'");
	TwAddVarRW(overlays_ptr->settings, "Reflection and Refraction", TW_TYPE_BOOLCPP, &SETTINGS.stg.refl_refr, "group='Graphics settings'");
	TwAddVarRW(overlays_ptr->settings, "Volumetric fog", TW_TYPE_BOOLCPP, &SETTINGS.stg.fog, "group='Graphics settings'");
	TwAddVarRW(overlays_ptr->settings, "Blur", TW_TYPE_FLOAT, &SETTINGS.stg.motion_blur, "min=0 step=0.001 max=0.75 group='Graphics settings'");
	TwAddVarRW(overlays_ptr->settings, "Exposure", TW_TYPE_FLOAT, &SETTINGS.stg.exposure, "min=0 max=5 step=0.001 group='Graphics settings'");
	TwAddVarRW(overlays_ptr->settings, "Bloom Treshold", TW_TYPE_FLOAT, &SETTINGS.stg.bloom_treshold, "min=0 max=5 step=0.001 group='Graphics settings'");
	TwAddVarRW(overlays_ptr->settings, "Bloom Intensity", TW_TYPE_FLOAT, &SETTINGS.stg.bloom_intensity, "min=0 max=5 step=0.001 group='Graphics settings'");
	TwAddVarRW(overlays_ptr->settings, "Bloom Radius", TW_TYPE_FLOAT, &SETTINGS.stg.bloom_radius, "min=1 max=10 step=0.1 group='Graphics settings'");


	TwAddVarRW(overlays_ptr->settings, "Language", Languages, &language_choise, "group='Gameplay settings'");
	TwEnumVal marble_type[] = { { 0, "Glass"  },
								{ 1,  "Metal" } };

	TwType Marble_type = TwDefineEnum("Marble type", marble_type, 2);
	TwAddVarRW(overlays_ptr->settings, "Marble type", Marble_type, &scene->MarbleType, "group='Gameplay settings'");
	TwAddVarRW(overlays_ptr->settings, "Mouse sensitivity", TW_TYPE_FLOAT, &SETTINGS.stg.mouse_sensitivity, "min=0.001 max=0.02 step=0.001 group='Gameplay settings'");
	TwAddVarRW(overlays_ptr->settings, "Wheel sensitivity", TW_TYPE_FLOAT, &SETTINGS.stg.wheel_sensitivity, "min=0.01 max=0.5 step=0.01 group='Gameplay settings'");
	TwAddVarRW(overlays_ptr->settings, "Music volume", TW_TYPE_FLOAT, &SETTINGS.stg.music_volume, "min=0 max=100 step=1 group='Gameplay settings'");
	TwAddVarRW(overlays_ptr->settings, "FX volume", TW_TYPE_FLOAT, &SETTINGS.stg.fx_volume, "min=0 max=100 step=1 group='Gameplay settings'");
	TwAddVarRW(overlays_ptr->settings, "Target FPS", TW_TYPE_FLOAT, target_fps, "min=24 max=144 step=1 group='Gameplay settings'");
	TwAddVarRW(overlays_ptr->settings, "Camera size", TW_TYPE_FLOAT, &scene->camera_size, "min=0 max=10 step=0.001 group='Gameplay settings'");
	TwAddVarRW(overlays_ptr->settings, "Camera speed(Free mode)", TW_TYPE_FLOAT, &scene->free_camera_speed, "min=0 max=10 step=0.001 group='Gameplay settings'");

	TwAddButton(overlays_ptr->settings, "Apply", ApplySettings, NULL, " label='Apply settings'  ");


	int barPos1[2] = { 16, 250 };

	TwSetParam(overlays_ptr->settings, NULL, "position", TW_PARAM_INT32, 2, &barPos1);

	TwCopyStdStringToClientFunc(CopyStdStringToClient);

	overlays_ptr->level_editor = TwNewBar("LevelEditor");
	Level *copy = &scene->level_copy;

	TwAddVarRW(overlays_ptr->level_editor, "Level Name", TW_TYPE_STDSTRING, &copy->txt, "");
	TwAddVarRW(overlays_ptr->level_editor, "Level Description", TW_TYPE_STDSTRING, &copy->desc, "");

	TwAddButton(overlays_ptr->level_editor, "Save", SaveLevel, NULL,
		" label='Save Level'  ");

	TwAddButton(overlays_ptr->level_editor, "Set Marble", MarbleSet, NULL,
		" label='Set Marble Position'  help='Click on the fractal to place' ");

	TwAddButton(overlays_ptr->level_editor, "Set Flag", FlagSet, NULL,
		" label='Set Flag Position'  help='Click on the fractal to place' ");

	TwAddVarRW(overlays_ptr->level_editor, "Flag Position", TW_TYPE_DIR3F, copy->end_pos.data(), "");
	TwAddVarRW(overlays_ptr->level_editor, "Marble Position", TW_TYPE_DIR3F, copy->start_pos.data(), "");
	TwAddVarRW(overlays_ptr->level_editor, "Marble Radius(Scale)", TW_TYPE_FLOAT, &copy->marble_rad, "min=0 max=10 step=0.001 ");

	
	TwAddVarRW(overlays_ptr->level_editor, "Level music", Level_music, &music_id, "");

	TwAddButton(overlays_ptr->level_editor, "Play", PlayMusic, NULL, " label='Play/Stop current music'  ");

	
	TwAddVarRW(overlays_ptr->level_editor, "Play level after finish(TODO)", Levels, &copy->link_level, "");

	TwAddVarRW(overlays_ptr->level_editor, "Sun direction", TW_TYPE_DIR3F, copy->light_dir.data(), "group='Level parameters'");
	TwAddVarRW(overlays_ptr->level_editor, "Sun color", TW_TYPE_DIR3F, copy->light_col.data(), "group='Level parameters'");
	//TwAddVarRW(level_editor, "Background color", TW_TYPE_DIR3F, copy->background_col.data(), "group='Level parameters'");
	TwAddVarRW(overlays_ptr->level_editor, "Gravity strength", TW_TYPE_FLOAT, &copy->gravity, "min=-0.5 max=0.5 step=0.0001 group='Level parameters'");
	TwAddVarRW(overlays_ptr->level_editor, "Kill y position (restart level)", TW_TYPE_FLOAT, &copy->kill_y, "min=-100 max=100 step=0.1 group='Level parameters'");
	TwAddVarRW(overlays_ptr->level_editor, "Is planet", TW_TYPE_BOOLCPP, &copy->planet, "group='Level parameters'");
	TwAddVarRW(overlays_ptr->level_editor, "Starting look direction angle", TW_TYPE_FLOAT, &copy->start_look_x, "min=-3.14159 max=3.14159 step=0.01 group='Level parameters'");

	overlays_ptr->fractal_editor = TwNewBar("FractalEditor");

	TwAddVarRW(overlays_ptr->fractal_editor, "PBR roughness", TW_TYPE_FLOAT, &copy->PBR_roughness, "min=0 max=1 step=0.001 group='Fractal Material'");
	TwAddVarRW(overlays_ptr->fractal_editor, "PBR metallic", TW_TYPE_FLOAT, &copy->PBR_metal, "min=0 max=1 step=0.001 group='Fractal Material'");
	float *p = copy->params.data();
	TwAddVarRW(overlays_ptr->fractal_editor, "Fractal Iterations", TW_TYPE_INT32, &copy->FractalIter, "min=1 max=20 step=1 group='Fractal Coefficients'");
	TwAddVarRW(overlays_ptr->fractal_editor, "Fractal Scale", TW_TYPE_FLOAT, p, "min=0 max=5 step=0.0001 group='Fractal Coefficients'");
	TwAddVarRW(overlays_ptr->fractal_editor, "Fractal Angle1", TW_TYPE_FLOAT, p + 1, "min=-10 max=10 step=0.0001 group='Fractal Coefficients'");
	TwAddVarRW(overlays_ptr->fractal_editor, "Fractal Angle2", TW_TYPE_FLOAT, p + 2, "min=-10 max=10 step=0.0001  group='Fractal Coefficients'");
	TwAddVarRW(overlays_ptr->fractal_editor, "Fractal Shift", TW_TYPE_DIR3F, p + 3, "group='Fractal Coefficients'");
	TwAddVarRW(overlays_ptr->fractal_editor, "Fractal Color", TW_TYPE_DIR3F, p + 6, "group='Fractal Coefficients'");

	TwAddVarRW(overlays_ptr->fractal_editor, "Fractal Animation1", TW_TYPE_FLOAT, &copy->anim_1, "min=0 max=0.5 step=0.0001 group='Fractal Animation'");
	TwAddVarRW(overlays_ptr->fractal_editor, "Fractal Animation2", TW_TYPE_FLOAT, &copy->anim_2, "min=0 max=0.5 step=0.0001 group='Fractal Animation'");
	TwAddVarRW(overlays_ptr->fractal_editor, "Fractal Animation3", TW_TYPE_FLOAT, &copy->anim_3, "min=0 max=0.5 step=0.0001 group='Fractal Animation'");

	overlays_ptr->flaunch = TwNewBar("First_launch");

	TwAddVarRW(overlays_ptr->flaunch, "Rendering resolution", Resolutions, &render_resolution, "");
	TwAddVarRW(overlays_ptr->flaunch, "Screenshot resolution", Resolutions, &screenshot_resolution, "");
	TwAddVarRW(overlays_ptr->flaunch, "Language", Languages, &language_choise, "");
	TwAddButton(overlays_ptr->flaunch, "OK", InitialOK, NULL, " label='OK'  ");
	TwSetParam(overlays_ptr->flaunch, NULL, "position", TW_PARAM_INT32, 2, &barPos1);

	TwDefine(" GLOBAL fontsize=3 ");
	TwDefine("LevelEditor visible=false size='420 350' color='0 80 230' alpha=210 label='Level editor' valueswidth=200");
	TwDefine("FractalEditor visible=false size='420 350' color='0 120 200' alpha=210 label='Fractal editor' valueswidth=200");
	TwDefine("Settings color='255 128 0' alpha=210 size='420 350' valueswidth=200");
	TwDefine("First_launch visible=false color='0 0 0' alpha=255 size='500 200' valueswidth=300");
	TwDefine("Statistics color='0 128 255' alpha=210 size='420 160' valueswidth=200");
}