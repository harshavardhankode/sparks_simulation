/*
  Sparks simulation

  Written by - 
               Harshavardhan Kode
*/

#include "spark.hpp"
#include "scene.hpp"
#include "texture.hpp"

GLuint shaderProgram,shaderProgram2;
int counter;
glm::mat4 rotation_matrix;
glm::mat4 projection_matrix;
glm::mat4 c_rotation_matrix;
glm::mat4 lookat_matrix;

glm::mat4 model_matrix;
glm::mat4 view_matrix;


glm::mat4 modelview_matrix;

GLuint uModelViewMatrix,uModelViewMatrix2;

sceneObject* sawBlade,*slab,*ground,*box,*bunny,*sphere,*cone;
GLuint sawBladeTex, slabTex, groundTex, woodTex;

std::vector<spark*> sparksList,newSparks;

int mode;
// angular velocity
float sawBladeRot;
bool recording=false;
unsigned int framenum=0;
unsigned char *pRGB;

int SCREEN_WIDTH=1280;
int SCREEN_HEIGHT=720;


//-----------------------------------------------------------------
//Function to render frame to images
void capture_frame(unsigned int framenum)
{
  //global pointer float *pRGB
  pRGB = new unsigned char [3 * (SCREEN_WIDTH+1) * (SCREEN_HEIGHT + 1) ];


  // set the framebuffer to read
  //default for double buffered
  glReadBuffer(GL_BACK);

  glPixelStoref(GL_PACK_ALIGNMENT,1);//for word allignment
  
  glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pRGB);
  char filename[200];
  sprintf(filename,"images/mode%d/frame_%04d.ppm",mode,framenum);
  std::ofstream out(filename, std::ios::out);
  out<<"P6"<<std::endl;
  out<<SCREEN_WIDTH<<" "<<SCREEN_HEIGHT<<" 255"<<std::endl;
  out.write(reinterpret_cast<char const *>(pRGB), (3 * (SCREEN_WIDTH+1) * (SCREEN_HEIGHT + 1)) * sizeof(int));
  out.close();

  //function to store pRGB in a file named count
  delete pRGB;
}



void initBuffersGL(void)
{

  // Load shaders and use the resulting shader program
  std::string vertex_shader_file("07_vshader.glsl");
  std::string fragment_shader_file("07_fshader.glsl");

  std::vector<GLuint> shaderList;
  shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
  shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));

  shaderProgram = csX75::CreateProgramGL(shaderList);

  // getting the attributes from the shader program
  vPosition = glGetAttribLocation( shaderProgram, "vPosition" );
  texCoord = glGetAttribLocation( shaderProgram, "texCoord" ); 
  vNormal = glGetAttribLocation( shaderProgram, "vNormal" ); 
  uModelViewMatrix = glGetUniformLocation( shaderProgram, "uModelViewMatrix");

  //shaderfor sparks
  shaderList.clear();
  shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, "spark_vshader.glsl"));
  shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, "spark_fshader.glsl"));

  shaderProgram2 = csX75::CreateProgramGL(shaderList);

  sparkPosition = glGetAttribLocation( shaderProgram2, "vPosition" );
  sparkColor = glGetAttribLocation( shaderProgram2, "vColor" );
  uModelViewMatrix2 = glGetUniformLocation( shaderProgram2, "uModelViewMatrix");

  // Load Textures 
  sawBladeTex=LoadTexture("assets/sawblade.bmp",915,915);
  slabTex = LoadTexture("assets/sawblade.bmp",500,300);
  groundTex = LoadTexture("assets/ground_tile.bmp",1000,1000);
  woodTex = LoadTexture("assets/wood.bmp",256,256);
  sawBlade = new sceneObject;
  sawBlade->readOBJ("assets/sawblade.obj");
  sawBladeRot = 0.0;


  slab = new sceneObject;
  slab->readOBJ("assets/slab.obj");

  ground = new sceneObject;
  ground->readOBJ("assets/ground.obj");

  box = new sceneObject;
  box->readOBJ("assets/cube.obj");

  bunny = new sceneObject;
  bunny->readOBJ("assets/stanford_bunny.obj");

  sphere = new sceneObject;
  sphere->readOBJ("assets/sphere.obj");

  cone = new sceneObject;
  cone->readOBJ("assets/cone.obj");

  if(mode == 2){
    sphere->translate(-1.0,0,0.5);
    box->translate(-1.0,0,-0.5);
    bunny->translate(0.5,0.0,0.0);
  }
}

void renderGL(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  matrixStack.clear();

  //Creating the lookat and the up vectors for the camera
  c_rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(c_xrot), glm::vec3(1.0f,0.0f,0.0f));
  c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_yrot), glm::vec3(0.0f,1.0f,0.0f));
  c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_zrot), glm::vec3(0.0f,0.0f,1.0f));

  glm::vec4 c_pos = glm::vec4(c_xpos,c_ypos,c_zpos, 1.0)*c_rotation_matrix;
  glm::vec4 c_up = glm::vec4(c_up_x,c_up_y,c_up_z, 1.0)*c_rotation_matrix;
  //Creating the lookat matrix
  lookat_matrix = glm::lookAt(glm::vec3(c_pos),glm::vec3(0.0),glm::vec3(c_up));

  //creating the projection matrix
  if(enable_perspective)
    projection_matrix = glm::frustum(-8.0, 8.0, -4.5, 4.5, 1.0, 7.0);
    //projection_matrix = glm::perspective(glm::radians(90.0),1.0,0.1,5.0);
  else
    projection_matrix = glm::ortho(-8.0, 8.0, -4.5, 4.5, -100.0, 100.0);

  view_matrix = projection_matrix*lookat_matrix;

  matrixStack.push_back(view_matrix);

  glUseProgram( shaderProgram );

  glBindTexture(GL_TEXTURE_2D, sawBladeTex);

  //saw blade rotation
  matrixStack.push_back(glm::translate(glm::mat4(1.0f),glm::vec3(0,(0.996216297 + 2.0037837)/2,0)));
  matrixStack.push_back(glm::rotate(glm::mat4(1.0f), glm::radians(sawBladeRot), glm::vec3(0.0f,0.0f,-1.0f)));
  matrixStack.push_back(glm::translate(glm::mat4(1.0f),glm::vec3(0,-(0.996216297 + 2.0037837)/2,0)));
  sawBlade->render();
  matrixStack.pop_back();
  matrixStack.pop_back();
  matrixStack.pop_back();

  glBindTexture(GL_TEXTURE_2D, slabTex);
  slab->render();


  glBindTexture(GL_TEXTURE_2D, woodTex);
  if(mode == 0){

  }else if(mode == 1){
    box->render();
  } else if(mode == 2){
    sphere->render();
    box->render();
    bunny->render();
  } 
  //box->render();
  //bunny->render();
  //sphere->render();
  //cone->render();

  glBindTexture(GL_TEXTURE_2D, groundTex);
  ground->render();


  glUseProgram( shaderProgram2 ); 
  for(int i=0;i<sparksList.size();i++){
    sparksList[i]->render();
  }
}

void step(float timestep){

  //rotating the sawblade
  sawBladeRot = sawBladeRot + timestep*2000;

  //handling sparks
  // 3 of 10 frames produce sparks
  //cout<<rand()%10<<endl;
  newSparks.clear();
  if((rand() % 10) < 10 /*&&counter == 1*/ ){

    sparksList.push_back(new spark(-0.238942921, 1.33096933,0.0,0.6,8));
  }

  for(int i=0;i<sparksList.size();i++){
    if(sparksList[i]->dead()){
      //cout<<"dead \n";
      delete(sparksList[i]);
      sparksList.erase(sparksList.begin() + i);
      i--;
    } else {
      
      sparksList[i]->step(timestep);
    }

  }
  sparksList.insert(sparksList.end(),newSparks.begin(),newSparks.end());
  cout<<sparksList.size()<<endl;



  
}

int main(int argc, char** argv)
{
  //set mode
  if(argc == 1){
    mode = 0;
  }
  else if(argc == 2){
    mode= (argv[1][0] - '0');
  } else if (argc == 3){
    mode= (argv[1][0] - '0');
    if(argv[2][0] == 'r'){
      recording= true;
    }
  }


  //! The pointer to the GLFW window
  GLFWwindow* window;

  //! Setting up the GLFW Error callback
  glfwSetErrorCallback(csX75::error_callback);

  //! Initialize GLFW
  if (!glfwInit())
    return -1;

  //We want OpenGL 4.0
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  //This is for MacOSX - can be omitted otherwise
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
  //We don't want the old OpenGL 
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

  //! Create a windowed mode window and its OpenGL context
  window = glfwCreateWindow(1280, 720, "Sparks simulation", NULL, NULL);
  if (!window)
    {
      glfwTerminate();
      return -1;
    }
  
  //! Make the window's context current 
  glfwMakeContextCurrent(window);

  //Initialize GLEW
  //Turn this on to get Shader based OpenGL
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (GLEW_OK != err)
    {
      //Problem: glewInit failed, something is seriously wrong.
      std::cerr<<"GLEW Init Failed : %s"<<std::endl;
    }

  //Print and see what context got enabled
  std::cout<<"Vendor: "<<glGetString (GL_VENDOR)<<std::endl;
  std::cout<<"Renderer: "<<glGetString (GL_RENDERER)<<std::endl;
  std::cout<<"Version: "<<glGetString (GL_VERSION)<<std::endl;
  std::cout<<"GLSL Version: "<<glGetString (GL_SHADING_LANGUAGE_VERSION)<<std::endl;

  //Keyboard Callback
  glfwSetKeyCallback(window, csX75::key_callback);
  //Framebuffer resize callback
  glfwSetFramebufferSizeCallback(window, csX75::framebuffer_size_callback);

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPointSize(2);
  //Initialize GL state
  csX75::initGL();
  initBuffersGL();
  counter = 0;
  // Loop until the user closes the window


  while (glfwWindowShouldClose(window) == 0)
    {
       
      // Render here

      step(0.0005);
      renderGL();
      counter++;
      //if(counter>100){
      //  break;
      //}
      // Swap front and back buffers
      if (recording){
        capture_frame(framenum++);
        if(framenum>2000){
          break;
        }

      }
      glfwSwapBuffers(window);
      
      // Poll for and process events
      glfwPollEvents();
    }
  
  glfwTerminate();
  return 0;
}

//-------------------------------------------------------------------------

