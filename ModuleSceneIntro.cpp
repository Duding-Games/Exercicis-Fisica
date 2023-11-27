#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	circle = box = rick = NULL;
	ray_on = false;
	sensed = false;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	circle = App->textures->Load("pinball/wheel.png"); 
	box = App->textures->Load("pinball/crate.png");
	rick = App->textures->Load("pinball/rick_head.png");
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");

	/*sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);*/
	circles.add(App->physics->CreateCircle(SCREEN_WIDTH/ 2,SCREEN_HEIGHT/2, 200));
	circles.getFirst()->data->body->SetGravityScale(0);
	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

update_status ModuleSceneIntro::Update(){



	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 25));
		circles.getLast()->data->listener = this;
		circles.getLast()->data->body->SetGravityScale(0);
	}

	p2List_item<PhysBody*>* c = circles.getFirst();


	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		if(c->data->Contains(App->input->GetMouseX(), App->input->GetMouseY()))
			App->renderer->Blit(circle, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	circles.getFirst()->data->body->SetTransform(b2Vec2(PIXEL_TO_METERS(SCREEN_WIDTH / 2), PIXEL_TO_METERS (SCREEN_HEIGHT / 2)), 0);

	if (circles.count() > 1) {
		for (int i = 0; i < circles.count(); i++)
		{
			PhysBody* circleBody;
			circles.at(i, circleBody);

			b2Vec2 CPos = circleBody->body->GetWorldCenter();
			float CMass = circleBody->body->GetMass();


			for (int j = i+1; j < circles.count(); j++) 
			{
				PhysBody* circleBody2;
				circles.at(j, circleBody2);

				b2Vec2 CPos2 = circleBody2->body->GetWorldCenter();
				float CMass2 = circleBody2->body->GetMass();

				b2Vec2 force = App->physics->Gravity(CMass, CMass2, circleBody->body, circleBody2->body);
				circleBody->body->ApplyForce(force, CPos, true);
				circleBody2->body->ApplyForce(-force, CPos2, true);

			}


		}
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	App->audio->PlayFx(bonus_fx);

	/*
	if(bodyA)
	{
		bodyA->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}

	if(bodyB)
	{
		bodyB->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}*/
}
