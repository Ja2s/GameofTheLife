#include "ofthelife.h"

void lire_grille(char *filename, int grille[MAX_SIZE][MAX_SIZE], int *n, int *m) {
	FILE *file;
	char ligne[MAX_SIZE];
	
	*n = 0;
	file = fopen(filename, "r");
	if (!file) {
		perror("Erreur lors de l'ouverture du fichier");
		exit(EXIT_FAILURE);
	}
	while (fgets(ligne, sizeof(ligne), file)) {
		*m = 0;
		for (int i = 0; ligne[i] != '\0' && ligne[i] != '\n'; i++) {
			grille[*n][i] = ligne[i] - '0';
			(*m)++;
		}
		(*n)++;
	}
	fclose(file);
}

void mise_a_jour_grille(int grille[MAX_SIZE][MAX_SIZE], int n, int m) {
	int temp[MAX_SIZE][MAX_SIZE];

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			int voisins_vivants = 0;
			for (int x = -1; x <= 1; x++) {
				for (int y = -1; y <= 1; y++) {
					if (x == 0 && y == 0) continue;
					int ni = i + x, nj = j + y;
					if (ni >= 0 && ni < n && nj >= 0 && nj < m) {
						voisins_vivants += grille[ni][nj];
					}
				}
			}
			if (grille[i][j] == 1) {
				temp[i][j] = (voisins_vivants == 2 || voisins_vivants == 3) ? 1 : 0;
			} else {
				temp[i][j] = (voisins_vivants == 3) ? 1 : 0;
			}
		}
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			grille[i][j] = temp[i][j];
		}
	}
}

void render_grid(SDL_Renderer *renderer, int grille[MAX_SIZE][MAX_SIZE], int n, int m) {
	// Dessiner le fond de la grille
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	// Dessiner les cellules vivantes avec une opacité complète
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			if (grille[i][j] == 1) {
				SDL_Rect cell = {j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE};
				SDL_RenderFillRect(renderer, &cell);
			}
		}
	}

	// Définir le mode de mélange pour ajuster l'opacité des lignes de la grille
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	// Dessiner la grille avec l'opacité souhaitée
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, GRID_OPACITY);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			SDL_Rect cell = {j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE};
			SDL_RenderDrawRect(renderer, &cell);
		}
	}

	SDL_RenderPresent(renderer);
}

int main() {
	int grille[MAX_SIZE][MAX_SIZE] = {0};
	int n = MAX_SIZE, m = MAX_SIZE;
	int running = 1, simulation = 0;
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Event event;

	lire_grille("map.ber", grille, &n, &m);

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Erreur lors de l'initialisation de la SDL : %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	int window_width = m * CELL_SIZE;
	int window_height = n * CELL_SIZE;

	window = SDL_CreateWindow("Jeu de la Vie",
							  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
							  window_width, window_height, SDL_WINDOW_SHOWN);
	if (!window) {
		fprintf(stderr, "Erreur lors de la création de la fenêtre : %s\n", SDL_GetError());
		SDL_Quit();
		return EXIT_FAILURE;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		fprintf(stderr, "Erreur lors de la création du renderer : %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = 0;
			} else if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_SPACE) {
					simulation = !simulation; // Inverse l'état de la simulation à chaque pression de la barre d'espace
				} else if (event.key.keysym.sym == SDLK_ESCAPE) {
					running = 0; // Quitter si la touche Echap est enfoncée
				}
			} else if (event.type == SDL_MOUSEBUTTONDOWN && !simulation) {
				int x = event.button.x / CELL_SIZE;
				int y = event.button.y / CELL_SIZE;
				if (y < n && x < m) {
					grille[y][x] = !grille[y][x];
				}
			}
		}

		render_grid(renderer, grille, n, m);

		if (simulation) {
			mise_a_jour_grille(grille, n, m);
			SDL_Delay(100);
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
