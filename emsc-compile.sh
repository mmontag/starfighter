set +e
set +x

#em++ \
#	Starfighter/Action.h \
#	Starfighter/Animation.h \
#	Starfighter/AssetCache.h \
#	Starfighter/Asteroid.h \
#	Starfighter/Asteroid3D.h \
#	Starfighter/Bullet.h \
#	Starfighter/Enemy.h \
#	Starfighter/ExplodeAction.h \
#	Starfighter/Game.h \
#	Starfighter/GameObject.h \
#	Starfighter/GameObjectFactory.h \
#	Starfighter/glew.h \
#	Starfighter/HUD.h \
#	Starfighter/Map.h \
#	Starfighter/Mesh.h \
#	Starfighter/Missile.h \
#	Starfighter/Model.h \
#	Starfighter/OpenGLRenderer.h \
#	Starfighter/Player.h \
#	Starfighter/PlayerDeathAction.h \
#	Starfighter/Point.h \
#	Starfighter/SDL_gpu_OpenGL_1.h \
#	Starfighter/SDL_gpu.h \
#	Starfighter/Shader.h \
#	Starfighter/Starfield.h \
#	Starfighter/Text.h \
#	Starfighter/Texture.h \
#	Starfighter/Timer.h \
#	Starfighter/tiny_obj_loader.h \
#	Starfighter/tmx.h \
em++ \
	Starfighter/Action.cpp \
	Starfighter/GameObjectFactory.cpp \
	Starfighter/main.cpp \
	Starfighter/Player.cpp \
	Starfighter/Texture.cpp \
  /Users/montag/src/tmx/build-emscripten/libtmx.a \
  /Users/montag/Downloads/libxml2-2.10.3/build-emscripten/libxml2.a \
	-s USE_SDL=2 \
  -s USE_SDL_IMAGE=2 \
	-s USE_SDL_MIXER=2 \
  -s USE_MODPLUG=1 \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s SDL2_IMAGE_FORMATS='["png"]' \
	--preload-file Sounds@/ \
	--preload-file Sprites@/ \
  -O2 \
	-o starfighter.html

