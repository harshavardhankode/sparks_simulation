#include "object.hpp"



extern GLuint vPosition,texCoord,vNormal,uModelViewMatrix;
extern std::vector<glm::mat4> matrixStack;



sceneObject::sceneObject(){

}


void sceneObject::readOBJ(string filename){
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec4 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec4 > temp_normals;

	ifstream file(filename.c_str());
	string temp,temp2;
	while( getline(file,temp)){

		string header;
		stringstream curr_line(temp);
		cout<<temp<<endl;
		curr_line>>header;
		//cout<<header<<endl;
	    if ( header == "v" ){
		    glm::vec4 vertex;		    
		    curr_line>>	vertex.x >> vertex.y >> vertex.z;
		    vertex.w = 1.0;
		    //cout<<vertex.x<<vertex.y<<vertex.z<<endl;
		    temp_vertices.push_back(vertex);
		}else if ( header == "vt"){
		    glm::vec2 uv;
		    curr_line>>	uv.x >> uv.y;
		    temp_uvs.push_back(uv);

		}else if ( header == "vn"){
		    glm::vec4 normal;
		    curr_line>> normal.x>>normal.y>>normal.z;
		    normal.w = 1.0;
		    temp_normals.push_back(normal);

		}else if ( header == "f" ){
		    
		    std::vector< glm::vec3 > vlist;

			while(curr_line >> temp2){
				glm::vec3 v;

				v.x = atoi( (temp2.substr(0,temp2.find("/"))).c_str() );
				cout<<v.x<<" ";
				temp2 = temp2.substr(temp2.find("/")+1);
				v.y = atoi( (temp2.substr(0,temp2.find("/"))).c_str() );
				cout<<v.y<<" ";
				if(temp2.find("/")!=string::npos){
					temp2 = temp2.substr(temp2.find("/")+1);
					v.z = atoi(temp2.c_str());
					cout<<v.z<<endl;
				}
				else{
					v.z = 1;
					cout<<v.z<<endl;
				}
				vlist.push_back(v);
			}

			for(int i=0;i<vlist.size() -2 ;i++){
			    vertexIndices.push_back(vlist[0].x);
			    vertexIndices.push_back(vlist[i+1].x);
			    vertexIndices.push_back(vlist[i+2].x);
			    uvIndices.push_back(vlist[0].y);
			    uvIndices.push_back(vlist[i+1].y);
			    uvIndices.push_back(vlist[i+2].y);
			    normalIndices.push_back(vlist[0].z);
			    normalIndices.push_back(vlist[i+1].z);
			    normalIndices.push_back(vlist[i+2].z);

			}
			if(normals.empty()){
				// for no normal files
				temp_normals.push_back(glm::vec4 (1.0,0.0,0.0,1.0));
			}

		}
	}
	
	for( unsigned int i=0; i<vertexIndices.size(); i++ ){

		glm::vec4 vertex = temp_vertices[vertexIndices[i]-1];
		vertices.push_back(vertex);
	}	
	for( unsigned int i=0; i<uvIndices.size(); i++ ){
		glm::vec2 uv = temp_uvs[uvIndices[i]-1];
		uvs.push_back(uv);
	}
	for( unsigned int i=0; i<normalIndices.size(); i++ ){	
		glm::vec4 normal = temp_normals[normalIndices[i]-1];
		normals.push_back(normal);
	}

	/*num_vertices = num_v;
	vertex_buffer_size = v_size;
	color_buffer_size = c_size;*/
	// initialize vao and vbo of the object;


	//Ask GL for a Vertex Attribute Objects (vao)
	glGenVertexArrays (1, &vao);
	//Ask GL for aVertex Buffer Object (vbo)
	glGenBuffers (1, &vbo);

	//bind them
	glBindVertexArray (vao);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);

	//Copy the points into the current buffer
	glBufferData (GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4) + uvs.size()* sizeof(glm::vec2) + normals.size() * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
	glBufferSubData( GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec4), &vertices[0] );
	glBufferSubData( GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4), uvs.size()* sizeof(glm::vec2), &uvs[0]);
	glBufferSubData( GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4) + uvs.size()* sizeof(glm::vec2),normals.size() * sizeof(glm::vec4), &normals[0] );
	// set up vertex array
	//Position
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	//Textures
	glEnableVertexAttribArray( texCoord );
	glVertexAttribPointer( texCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertices.size() * sizeof(glm::vec4) ) );

	
	//Normal
	glEnableVertexAttribArray( vNormal );
	glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertices.size() * sizeof(glm::vec4) + uvs.size()* sizeof(glm::vec2)) );

	
	//cout<<vPosition<< " "<<texCoord<<" "<<vNormal<<" "<<endl;
}

void sceneObject::render(){

	glm::mat4* ms_mult = multiply_stack(matrixStack);

	glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(*ms_mult));
	glBindVertexArray (vao);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	//cout<<glm::to_string(*ms_mult)<<endl;
	// for memory 
	delete ms_mult;
}


glm::mat4* multiply_stack(std::vector<glm::mat4> matStack){
	glm::mat4* mult;
	mult = new glm::mat4(1.0f);

	for(int i=0;i<matStack.size();i++){
		*mult = (*mult) * matStack[i];
	}	

	return mult;
}

void sceneObject::translate(float x, float y, float z){

	glm::vec4 t(x,y,z,0);

	for(int i=0;i<vertices.size();i++){
		vertices[i] = vertices[i] + t; 
	}
	updateBuffers();
}

void sceneObject::updateBuffers(){

	glBindVertexArray (vao);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);

	//Copy the points into the current buffer
	glBufferData (GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4) + uvs.size()* sizeof(glm::vec2) + normals.size() * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
	glBufferSubData( GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec4), &vertices[0] );
	glBufferSubData( GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4), uvs.size()* sizeof(glm::vec2), &uvs[0]);
	glBufferSubData( GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4) + uvs.size()* sizeof(glm::vec2),normals.size() * sizeof(glm::vec4), &normals[0] );
	// set up vertex array
	//Position
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	//Textures
	glEnableVertexAttribArray( texCoord );
	glVertexAttribPointer( texCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertices.size() * sizeof(glm::vec4) ) );

	
	//Normal
	glEnableVertexAttribArray( vNormal );
	glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertices.size() * sizeof(glm::vec4) + uvs.size()* sizeof(glm::vec2)) );


}
bool sceneObject::intersects(glm::vec3 currpos,glm::vec3 nextpos, glm::vec3& normal){

	// every object vertices list is 3 consecutive vertices made of faces  

	for(int i=0;i<vertices.size()-2;i+=3){

		glm::vec3 v1(vertices[i].x,vertices[i].y,vertices[i].z);
		glm::vec3 v2(vertices[i+1].x,vertices[i+1].y,vertices[i+1].z);
		glm::vec3 v3(vertices[i+2].x,vertices[i+2].y,vertices[i+2].z);


		glm::vec3 n = glm::cross(v2-v1,v3-v1);
		n = glm::normalize(n);

		if(glm::dot(n,currpos-v1)*glm::dot(n,nextpos-v1)>0){
			continue;
		}
		else{
			glm::vec3 proj= currpos - n*glm::dot(n,currpos-v1);
			//checking if projection is on the triangle
			if(glm::length(glm::cross(v2-v1,v3-v1)) < glm::length(glm::cross(v2-v1,proj-v1)) + glm::length(glm::cross(proj-v1,v3-v1)) ){
				continue;
			}
			else{
				normal = normalize(n*glm::dot(n,currpos-v1));
				return true;
			}

		}

	}

	return false;


}
