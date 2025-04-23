#ifndef BASE_GRAPHIC
#define BASE_GRAPHIC
#include "glm/glm.hpp"
#include "VulkanBuffer.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
struct BaseGraphic{
    VulkanBuffer index;
    VulkanBuffer vertex;
    uint32_t indexCount;
    uint32_t vertexCount;
    VkIndexType indexType = VK_INDEX_TYPE_UINT16;
    VkResult CreateVertexBuffer(VulkanDevice device, const void* data, VkDeviceSize size, uint32_t vertexCount, VkQueue graphics, VulkanPool pool, VkBufferUsageFlags additionalUsage = 0) {
        this->vertexCount = vertexCount;
        // 创建目标顶点缓冲区（设备本地内存）
        VkResult result = vertex.CreateBuffer(device, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT|additionalUsage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        if (result == VK_SUCCESS) {
            // 使用暂存缓冲区上传数据
            vertex.UpdateData(device, data, graphics, pool);
        }
        return result;
    }
    VkResult CreateIndexBuffer(VulkanDevice device, const void* data, VkDeviceSize size, VkQueue graphics, VulkanPool pool, VkIndexType indexType = VK_INDEX_TYPE_UINT16) {
        this->indexType = indexType;
        indexCount = static_cast<uint32_t>(indexType == VK_INDEX_TYPE_UINT16?size / sizeof(uint16_t):size / sizeof(uint32_t));
        // 创建目标索引缓冲区（设备本地内存）
        VkResult result = index.CreateBuffer(device, size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT,  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        if (result == VK_SUCCESS) {
            // 使用暂存缓冲区上传数据
            index.UpdateData(device, data, graphics, pool);
        }
        return result;
    }
    void Destroy(VkDevice device){
        indexCount = 0;
        vertexCount = 0;
        vertex.Destroy(device);
        index.Destroy(device);
    }
    void Bind(VkCommandBuffer command)const{
        VkDeviceSize offset = 0;
        if(vertex.buffer != VK_NULL_HANDLE)vkCmdBindVertexBuffers(command, 0, 1, &vertex.buffer, &offset);
        if(index.buffer != VK_NULL_HANDLE)vkCmdBindIndexBuffer(command, index.buffer, offset, indexType);
    }
    void Bind(VkCommandBuffer command, VkIndexType indexType)const{
        VkDeviceSize offset = 0;
        if(vertex.buffer != VK_NULL_HANDLE)vkCmdBindVertexBuffers(command, 0, 1, &vertex.buffer, &offset);
        if(index.buffer != VK_NULL_HANDLE)vkCmdBindIndexBuffer(command, index.buffer, offset, indexType);
    }
    void Draw(VkCommandBuffer command, uint32_t vertexOffset = 0, uint32_t firstIndex = 0)const{
        if(index.buffer != VK_NULL_HANDLE){
            vkCmdDrawIndexed(command, indexCount, 1, firstIndex, vertexOffset, 0);
        }
        else if(vertex.buffer != VK_NULL_HANDLE){
            vkCmdDraw(command, vertexCount, 1, vertexOffset, 0);
        }
    }
    inline auto MapVertex(VkDevice device, VkMemoryMapFlags flags = 0){
        void *data;
        vkMapMemory(device, vertex.memory, 0, vertex.size, flags, &data);
        return data;
    }
    inline void UnMapVertex(VkDevice device){
        vkUnmapMemory(device, vertex.memory);
    }
    inline auto MapIndex(VkDevice device, VkMemoryMapFlags flags = 0){
        void *data;
        vkMapMemory(device, index.memory, 0, index.size, flags, &data);
        return data;
    }
    inline void UnMapIndex(VkDevice device){
        vkUnmapMemory(device, index.memory);
    }
    inline uint32_t GetIndexCount(){
        return indexCount;
    }
    inline uint32_t GetVertexCount(){
        return vertexCount;
    }
};

struct VulkanCube:public BaseGraphic{
    void CreateCube(VulkanDevice device, const glm::vec3&color, VkQueue graphics, VulkanPool pool){
        const float vertices[] = {
            //正面
            -1.0f, -1.0f,  1.0f, color.x, color.y, color.z,//左下角//0
            1.0f, -1.0f,  1.0f, color.x, color.y, color.z,//右下角//1
            1.0f,  1.0f,  1.0f, color.x, color.y, color.z,//右上角//2
            -1.0f,  1.0f,  1.0f, color.x, color.y, color.z,//左上角//3
            //背面
            -1.0f, -1.0f, -1.0f, color.x, color.y, color.z,//左下角//4
            1.0f, -1.0f, -1.0f, color.x, color.y, color.z,//右下角//5
            1.0f,  1.0f, -1.0f, color.x, color.y, color.z,//右上角//6
            -1.0f,  1.0f, -1.0f, color.x, color.y, color.z,//左上角//7
        };
        const uint16_t indices[] = {
            0,1,2, 2,3,0,//正面
            1,5,6, 6,2,1, //右面
            7,6,5, 5,4,7, //背面
            4,0,3, 3,7,4, 
            4,5,1, 1,0,4, 
            3,2,6, 6,7,3,
        };
        CreateIndexBuffer(device, indices, sizeof(indices), graphics, pool);
        CreateVertexBuffer(device, vertices, sizeof(vertices), 8, graphics, pool);
    }
};
struct VulkanRect:public BaseGraphic{
    //主要用于2d游戏
    void CreateRect2D(VulkanDevice device, const glm::vec3&color, VkQueue graphics, VulkanPool pool){
        const float vertices[] = {
            // 位置     // 纹理
            .0f, 1.0f, .0f, color.x, color.y, color.z,//左下
            1.0f, .0f, .0f, color.x, color.y, color.z,//右上
            .0f, .0f, .0f, color.x, color.y, color.z, //左上

            1.0f, 1.0f, .0f, color.x, color.y, color.z//右下
        };
        const uint16_t indices[] = { 0, 1, 2, 0, 3, 1 };
        CreateIndexBuffer(device, indices, sizeof(indices), graphics, pool);
        CreateVertexBuffer(device, vertices, sizeof(vertices), 4, graphics, pool);
    }
    //主要用于3d游戏, 例如地板
    void CreateRect3D(VulkanDevice device, const glm::vec3&color, VkQueue graphics, VulkanPool pool){
        const uint16_t indices[] = { 0, 1, 2, 0, 3, 1 };
        const float vertices[] = {
            // 位置
            -1.0f, 0, 1.0f, color.x, color.y, color.z,
            1.0f, 0, -1.0f, color.x, color.y, color.z,
            -1.0f, 0, -1.0f, color.x, color.y, color.z,
            1.0f, 0, 1.0f, color.x, color.y, color.z
        };
        CreateIndexBuffer(device, indices, sizeof(indices), graphics, pool);
        CreateVertexBuffer(device, vertices, sizeof(vertices), 4, graphics, pool);
    }
};
struct VulkanCircle:BaseGraphic{
private:
    struct Vertex{
        glm::vec3 pos;
        glm::vec3 color;
        Vertex(){

        }
        Vertex(const glm::vec3&pos, const glm::vec3&color){
            this->pos = pos;
            this->color = color;
        }
    };
    void GetCircularVertex(const glm::vec3 &color, std::vector<Vertex> &vertices){
        Vertex v = Vertex(glm::vec3(.0f), color);
        vertices.push_back(v);
        for (float i = 0; i <= 360.0f; ++i){
            const float r = glm::radians(i);
            v.pos = glm::vec3(sin(r), cos(r), .0f);
            vertices.push_back(v);
        }
    }
public:
    //目前圆的大小只能通过glm::scale决定
    void CreateCircie(VulkanDevice device, const glm::vec3&color, VkQueue graphics, VulkanPool pool){
        std::vector<Vertex>vertices;
        GetCircularVertex(color, vertices);
        vertexCount = vertices.size();
        std::vector<uint16_t> Indices;
        for (size_t i = 1; i <= vertices.size(); ++i){
            Indices.push_back(0);
            Indices.push_back(i);
            Indices.push_back(i + 1);
        }
        CreateIndexBuffer(device, Indices.data(), sizeof(uint16_t) * Indices.size(), graphics, pool);
        CreateVertexBuffer(device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), graphics, pool);
    }
};
struct VulkanSphere:BaseGraphic{
    struct Vertex{
        glm::vec3 pos;
        glm::vec3 color;
        Vertex(){

        }
        Vertex(const glm::vec3&pos, const glm::vec3&color){
            this->pos = pos;
            this->color = color;
        }
    };
    const uint32_t SPHERE_DETAIL = 30;
    void GetVertices(float radius, const glm::vec3&color, std::vector<Vertex>&vertices){
        Vertex v;
        v.color = glm::vec3(1, 0, 0) * .5f;
        vertices.emplace_back(Vertex(glm::vec3(0.0f, 0.0f, radius), color));
        // 纬度细分（递增Z轴）
        for (int lat = 1; lat <= SPHERE_DETAIL; ++lat) {
            float theta = lat * M_PI / SPHERE_DETAIL; // [0, π]
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);
            
            // 经度细分（复平面XY映射）
            for (int lon = 0; lon <= SPHERE_DETAIL; ++lon) {
                float phi = lon * 2 * M_PI / SPHERE_DETAIL; // [0, 2π]
                float sinPhi = sin(phi);
                float cosPhi = cos(phi);
                
                // 球面坐标转笛卡尔坐标
                glm::vec3 pos(
                    radius * sinTheta * cosPhi, // X
                    radius * sinTheta * sinPhi, // Y
                    radius * cosTheta            // Z
                );
                v.pos = pos;
                v.color = pos;
                vertices.push_back(v);
            }
        }
        vertices.emplace_back(Vertex(glm::vec3(0.0f, 0.0f, -radius), color));        
    }
    void GetIndices(uint32_t vertexCount, std::vector<uint16_t>&indices){
        for (int lat = 0; lat < SPHERE_DETAIL; ++lat) {
            for (int lon = 0; lon < SPHERE_DETAIL; ++lon) {
                // 计算当前层和下一层的顶点索引
                int current = (lat == 0) ? 0 : ( (lat - 1) * (SPHERE_DETAIL + 1) + lon + 1 );
                int next = (lat == 0) ? 0 : ( (lat - 1) * (SPHERE_DETAIL + 1) + lon + 2 );
                int below = (lat == SPHERE_DETAIL - 1) ? vertexCount - 1 : ( lat * (SPHERE_DETAIL + 1) + lon + 1 );
                int belowNext = (lat == SPHERE_DETAIL - 1) ? vertexCount - 1 : ( lat * (SPHERE_DETAIL + 1) + lon + 2 );

                // 连接三角形
                if (lat > 0) { // 非北极层
                    indices.push_back(current);
                    indices.push_back(next);
                    indices.push_back(belowNext);
                    
                    indices.push_back(current);
                    indices.push_back(belowNext);
                    indices.push_back(below);
                } else { // 北极层到第一中间层
                    indices.push_back(0); // 北极
                    indices.push_back(lon + 1);
                    indices.push_back(lon + 2);
                }
            }
        }

        // 闭合南极层
        for (int lon = 0; lon < SPHERE_DETAIL; ++lon) {
            int current = (SPHERE_DETAIL - 2) * (SPHERE_DETAIL + 1) + lon + 1;
            int next = (lon == SPHERE_DETAIL - 1) ? (SPHERE_DETAIL - 2) * (SPHERE_DETAIL + 1) + 1 : current + 1;
            
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(vertexCount - 1); // 南极
        }
    }
public:
    void CreateSphere(VulkanDevice device, float radius, const glm::vec3&color, VkQueue graphics, VulkanPool pool){
        std::vector<Vertex>vertices;
        std::vector<uint16_t>indices;
        GetIndices(vertices.size(), indices);
        GetVertices(radius, color, vertices);
        if(vertex.buffer != VK_NULL_HANDLE || vertexCount != vertices.size()){
            Destroy(device.device);
        }
        if(index.buffer != VK_NULL_HANDLE || indices.size() != indexCount){
            index.Destroy(device.device);
        }
        CreateIndexBuffer(device, indices.data(), indices.size() * sizeof(uint16_t), graphics, pool);
        CreateVertexBuffer(device, vertices.data(), vertices.size() * sizeof(Vertex), vertices.size(), graphics, pool);
    }
};
struct VulkanCylinder:BaseGraphic{
private:
    struct Vertex{
        glm::vec3 pos;
        glm::vec3 color;
    };
    std::vector<Vertex> GetUnitCircleVertices(float radius, const glm::vec3&color){
        int sectorCount = 36;
        float sectorStep = 2 * M_PI / sectorCount;
        float sectorAngle = 0.0f;
        Vertex v;
        v.color = color;
        glm::vec3 position;
        std::vector<Vertex> unitCircleVertices;
        for (int i = 0; i <= sectorCount; ++i){
            sectorAngle = i * sectorStep;
            position.x = radius * cos(sectorAngle);
            position.y = 0.0f;
            position.z = radius * sin(sectorAngle);
            // normal.x = cos(sectorAngle);
            // normal.y = 0.0f;
            // normal.z = sin(sectorAngle);
            // tVertex.pos = position;
            unitCircleVertices.push_back(v);
        }
        return unitCircleVertices;
    }
    // generate vertices for a cylinder
    std::vector<Vertex>GetCylinderVertices(float radius, float height, const glm::vec3&color){
        auto unitVertices = GetUnitCircleVertices(radius, color);

        // 获取上、下圆周点数组
        std::vector<Vertex> vctTop;
        std::vector<Vertex> vctBot;

        Vertex tVertex;
        for(int i = 0; i < unitVertices.size(); ++i){
            tVertex.pos = unitVertices[i].pos;
            tVertex.pos.y = height;
            vctTop.push_back(tVertex);  

            tVertex.pos.y = 0.0f;
            vctBot.push_back(tVertex);     
        }

        assert(vctTop.size() >= 2);
        assert(vctBot.size() >= 2);

        std::vector<Vertex>vertices;
        // 圆柱侧面
        for(int i = 0; i < vctTop.size() - 1; ++i){
            // 左三角形
            vertices.push_back(vctTop[i]);
            vertices.push_back(vctBot[i]);
            vertices.push_back(vctBot[i+1]);
            
            // 右三角形
            vertices.push_back(vctTop[i]);
            vertices.push_back(vctTop[i+1]);
            vertices.push_back(vctBot[i+1]);
        }
        
        // 顶部圆形
        glm::vec3 position;
        for (int i = 0; i < vctTop.size() - 1; ++i){
            glm::vec3 position(0.0f, height, 0.0f);
            glm::vec3 normal(0.0f, 1.0f, 0.0f);
            tVertex.pos = position;
            vertices.push_back(tVertex);

            tVertex.pos = vctTop[i].pos;
            vertices.push_back(tVertex);

            tVertex.pos = vctTop[i+1].pos;
            vertices.push_back(tVertex);
        }

        // 底部圆形
        for (int i = 0; i < vctBot.size() - 1; ++i){
            glm::vec3 position(0.0f, 0.0f, 0.0f);
            glm::vec3 normal(0.0f, -1.0f, 0.0f);
            tVertex.pos = position;
            vertices.push_back(tVertex);
            tVertex.pos = vctBot[i].pos;
            vertices.push_back(tVertex);

            tVertex.pos = vctBot[i+1].pos;
            vertices.push_back(tVertex);
        }
        return vertices;
    }
public:
    void CreateCylinder(VulkanDevice device, float radius, float height, const glm::vec3&color, VkQueue graphics, VulkanPool pool){
        auto vertices = GetCylinderVertices(radius, height, color);
        CreateVertexBuffer(device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), graphics, pool);
    }
};
#endif