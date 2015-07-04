all:
	g++ playground.cpp car.cpp common/shader.cpp -o playground -lglfw -lX11 -lXrandr -lXi -lXxf86vm -lGL -lGLEW
	
clean:
	rm playground
