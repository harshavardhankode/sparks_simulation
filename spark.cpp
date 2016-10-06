#include "spark.hpp"
#include <stdlib.h>


using namespace std;

extern GLuint sparkPosition,sparkColor,uModelViewMatrix2;
extern std::vector<glm::mat4> matrixStack;
extern std::vector<spark*> newSparks;
extern sceneObject* box, *ground,*bunny,*sphere,*cone;
extern int mode;

spark::spark(float xpos, float ypos, float zpos, float maxlife,int max_length){

	x= xpos;y=ypos;z=zpos;
	spark_length =  2+ max_length/2 + rand() % max_length/2;
	vx = -10-rand()%30;
	vy = -10+rand()%30;
	vz = -10+ rand()%20;
	vz = vz*0.5;	
	life = maxlife/2 + (rand()%10/float(10))* (maxlife/2); 
	currlife = life;
	for(int i = 0; i < spark_length ; i++){

		spark_path.push_back(glm::vec4(x,y,z,1.0));
		spark_color.push_back(glm::vec4(1.0,0.5+0.5*((float)i/spark_length),0.5*((float)i/spark_length),1.0));
	}
	

	//Ask GL for a Vertex Attribute Objects (vao)
	glGenVertexArrays (1, &vao);
	//Ask GL for aVertex Buffer Object (vbo)
	glGenBuffers (1, &vbo);



}

spark::spark(float xpos, float ypos, float zpos, float maxlife, int max_length, float avx, float avy, float avz){

	x= xpos;y=ypos;z=zpos;
	spark_length = 2 + max_length/2 + rand() % max_length/2;
	vx = avx;
	vy = avy;
	vz = avz;	
	life = maxlife/2 + (rand()%10/float(10))* (maxlife/2);
	currlife = life;
	for(int i = 0; i < spark_length ; i++){

		spark_path.push_back(glm::vec4(x,y,z,1.0));
		spark_color.push_back(glm::vec4(1.0,0.5+0.5*((float)i/spark_length),0.5*((float)i/spark_length),1.0));
	}
	

	//Ask GL for a Vertex Attribute Objects (vao)
	glGenVertexArrays (1, &vao);
	//Ask GL for aVertex Buffer Object (vbo)
	glGenBuffers (1, &vbo);


}

spark::~spark(){
	glDeleteBuffers(1,&vbo);
	glDeleteVertexArrays(1,&vao);
}

void spark::step(float timestep){

	glm::vec3 oldpos(x,y,z);
	x = x+vx*timestep;
	y = y+vy*timestep;
	z = z+vz*timestep;
	glm::vec3 newpos(x,y,z);

	glm::vec3 v(vx,vy,vz);


	if(sqrt(glm::dot(v,v)) > 100){

		v = v - 0.001f*sqrt(glm::dot(v,v)-100)*v; // air resistance
		vx = v.x;
		vy = v.y;
		vz = v.z;
	}

	vy = vy- 100.0*timestep;  // gravity


	//moving
	spark_path.erase(spark_path.begin());
	spark_path.push_back(glm::vec4(x,y,z,1.0));

	currlife-=timestep;
	
	for(int i = 0; i < spark_length ; i++){
		spark_color[i].w = (currlife-0.1)/(float)life;	
	}

	/*if(detect_ground_collisions()){
		//vy = -0.05*(rand%10)vy; y = -y;
		//dead
		vy = -vy;
		//split spark into two conserving the momentum along x and z if it has decent y velocity
		if(vy>0.5){
			float dx = 0.5*(-5+rand()%10);
			float dz = 0.5*(-5+rand()%10);
			//random coeff of restitution
			vy = (0.5 + 0.025*(rand()%10))*vy; y = -y;
			//chose random direction
			newSparks.push_back(new spark(x,y,z,currlife,spark_path.size(),vx+dx,vy,vz+dz));
			newSparks.push_back(new spark(x,y,z,currlife,spark_path.size(),vx-dx,vy,vz-dz));
		}
		//dead
		currlife = 10;
	}*/

	glm::vec3 tempnormal;
	handle_object_collisions(oldpos,newpos,ground,tempnormal);

	if(mode == 0){

	}else if(mode == 1){
		handle_object_collisions(oldpos,newpos,box,tempnormal);
	} else if(mode == 2){
		handle_object_collisions(oldpos,newpos,box,tempnormal);
		handle_object_collisions(oldpos,newpos,sphere,tempnormal);
		handle_object_collisions(oldpos,newpos,bunny,tempnormal);
	} 
	//handle_object_collisions(oldpos,newpos,box,tempnormal);
	//handle_object_collisions(oldpos,newpos,bunny,tempnormal);
	//handle_object_collisions(oldpos,newpos,cone,tempnormal);
	//handle_object_collisions(oldpos,newpos,sphere,tempnormal);
}

bool spark::dead(){

	if(currlife<0.1){
		return true;
	}
	else 
		return false;

}

bool spark::detect_ground_collisions(){

	if(y<0)
		return true;
	else 
		return false;

}

bool spark::handle_object_collisions(glm::vec3 currpos,glm::vec3 nextpos,sceneObject* obj, glm::vec3& normal){

	if( obj->intersects(currpos,nextpos,normal)){

		//cout<<glm::to_string(normal)<<endl;
		glm::vec3 v(vx,vy,vz);


		v = v - (1.5f)*normal*(glm::dot(v,normal));

		vx = v.x;
		vy = v.y;
		vz = v.z;

		x=currpos.x,y=currpos.y,z=currpos.z;

		
		//split into two sparks
		if(glm::length(v)>0.5){

			glm::vec3 pdir = glm::cross(v,normal);


			//conserving momentum in direction perpendicular to normal
			float diverge = 0.1+rand()%10/(float)50;

			glm::vec3 v1 = v+diverge*pdir;
			glm::vec3 v2 = v-diverge*pdir;

			//two new sparks in random direction, conserving the momentum
			newSparks.push_back(new spark(x,y,z,currlife,spark_path.size(),v1.x,v1.y,v1.z));
			newSparks.push_back(new spark(x,y,z,currlife,spark_path.size(),v2.x,v2.y,v2.z));
		}

		currlife = 0.1;
	}

	return false;

}

void spark::render(){

	//bind them
	glBindVertexArray (vao);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);



	//Copy the points into the current buffer
	glBufferData (GL_ARRAY_BUFFER, spark_path.size() * sizeof(glm::vec4) + spark_color.size()* sizeof(glm::vec4) , NULL, GL_STATIC_DRAW);
	glBufferSubData( GL_ARRAY_BUFFER, 0, spark_path.size() * sizeof(glm::vec4), &spark_path[0] );
	glBufferSubData( GL_ARRAY_BUFFER, spark_path.size() * sizeof(glm::vec4), spark_color.size() * sizeof(glm::vec4), &spark_color[0]);
	// set up vertex array
	//Position
	glEnableVertexAttribArray( sparkPosition );
	glVertexAttribPointer( sparkPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	//Textures
	glEnableVertexAttribArray( sparkColor );
	glVertexAttribPointer( sparkColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(spark_path.size() * sizeof(glm::vec4) ) );


	glm::mat4* ms_mult = multiply_stack(matrixStack);

	glUniformMatrix4fv(uModelViewMatrix2, 1, GL_FALSE, glm::value_ptr(*ms_mult));
	glBindVertexArray (vao);
	glDrawArrays(GL_POINTS, 0, spark_path.size());

	//cout<<glm::to_string(*ms_mult)<<endl;
	// for memory 
	delete ms_mult;


}