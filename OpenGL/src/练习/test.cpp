#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

int main()
{
    glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 transf = glm::mat4(1.0f);
    transf = glm::translate(transf, glm::vec3(1.0f, 1.0f, 0.0f));
    vec = transf * vec;
    std::cout << vec.x << vec.y << vec.z << std::endl;
    system("pause");
    return 0;
}