//#include "YTE/Graphics/MeshLoader.hpp"
//#include "YTE/Graphics/TextureLoader.hpp"
//
//namespace YTE
//{
//  // Load the mesh with custom flags
//
//
//  // Loads the mesh with some default flags
//
//  bool MeshLoader::LoadMesh(const std::string & filename)
//  {
//    int flags = aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals;
//
//    return LoadMesh(filename, flags);
//  }
//
//  bool MeshLoader::LoadMesh(const std::string & filename, int flags)
//  {
//    pScene = Importer.ReadFile(filename.c_str(), flags);
//
//    if (pScene)
//    {
//      return InitFromScene(pScene, filename);
//    }
//    else
//    {
//      printf("Error parsing '%s': '%s'\n", filename.c_str(), Importer.GetErrorString());
//      return false;
//    }
//  }
//  bool MeshLoader::InitFromScene(const aiScene * pScene, const std::string & Filename)
//  {
//    mEntries.resize(pScene->mNumMeshes);
//
//    // Counters
//    for (unsigned int i = 0; i < mEntries.size(); i++)
//    {
//      mEntries[i].vertexBase = numVertices;
//      numVertices += pScene->mMeshes[i]->mNumVertices;
//    }
//
//    // Initialize the meshes in the scene one by one
//    for (unsigned int i = 0; i < mEntries.size(); i++)
//    {
//      const aiMesh* paiMesh = pScene->mMeshes[i];
//      InitMesh(i, paiMesh, pScene);
//    }
//
//    return true;
//  }
//  void MeshLoader::InitMesh(unsigned int index, const aiMesh * paiMesh, const aiScene * pScene)
//  {
//    mEntries[index].MaterialIndex = paiMesh->mMaterialIndex;
//
//    aiColor3D pColor(0.f, 0.f, 0.f);
//    pScene->mMaterials[paiMesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, pColor);
//
//    aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
//
//    for (unsigned int i = 0; i < paiMesh->mNumVertices; i++)
//    {
//      aiVector3D* pPos = &(paiMesh->mVertices[i]);
//      aiVector3D* pNormal = &(paiMesh->mNormals[i]);
//      aiVector3D *pTexCoord;
//      if (paiMesh->HasTextureCoords(0))
//      {
//        pTexCoord = &(paiMesh->mTextureCoords[0][i]);
//      }
//      else
//      {
//        pTexCoord = &Zero3D;
//      }
//
//      aiVector3D* pTangent = (paiMesh->HasTangentsAndBitangents()) ? &(paiMesh->mTangents[i]) : &Zero3D;
//      aiVector3D* pBiTangent = (paiMesh->HasTangentsAndBitangents()) ? &(paiMesh->mBitangents[i]) : &Zero3D;
//
//      Vertex v(glm::vec3(pPos->x, -pPos->y, pPos->z),
//               glm::vec2(pTexCoord->x, pTexCoord->y),
//               glm::vec3(pNormal->x, pNormal->y, pNormal->z),
//               glm::vec3(pTangent->x, pTangent->y, pTangent->z),
//               glm::vec3(pBiTangent->x, pBiTangent->y, pBiTangent->z),
//               glm::vec3(pColor.r, pColor.g, pColor.b)
//               );
//
//      mDimension.max.x = fmax(pPos->x, mDimension.max.x);
//      mDimension.max.y = fmax(pPos->y, mDimension.max.y);
//      mDimension.max.z = fmax(pPos->z, mDimension.max.z);
//
//      mDimension.min.x = fmin(pPos->x, mDimension.min.x);
//      mDimension.min.y = fmin(pPos->y, mDimension.min.y);
//      mDimension.min.z = fmin(pPos->z, mDimension.min.z);
//
//      mEntries[index].Vertices.push_back(v);
//    }
//
//    mDimension.size = mDimension.max - mDimension.min;
//
//    for (unsigned int i = 0; i < paiMesh->mNumFaces; i++)
//    {
//      const aiFace& Face = paiMesh->mFaces[i];
//
//      if (Face.mNumIndices != 3)
//      {
//        continue;
//      }
//
//      mEntries[index].Indices.push_back(Face.mIndices[0]);
//      mEntries[index].Indices.push_back(Face.mIndices[1]);
//      mEntries[index].Indices.push_back(Face.mIndices[2]);
//    }
//  }
//  vk::Result MeshLoader::createBuffer(vk::Device device, vk::PhysicalDeviceMemoryProperties deviceMemoryProperties, vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags, vk::DeviceSize size, vk::Buffer * buffer, vk::DeviceMemory * memory)
//  {
//    vk::MemoryAllocateInfo memAllocInfo;
//
//    vk::BufferCreateInfo bufferInfo; // = vkTools::initializers::bufferCreateInfo(usageFlags, size);
//    bufferInfo.setUsage(usageFlags);
//    bufferInfo.setSize(size);
//
//    *buffer = device.createBuffer(bufferInfo);
//    auto memReqs = device.getBufferMemoryRequirements(*buffer);
//
//    memAllocInfo.allocationSize = memReqs.size;
//    memAllocInfo.memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, deviceMemoryProperties, memoryPropertyFlags);
//
//    *memory = device.allocateMemory(memAllocInfo);
//    device.bindBufferMemory(*buffer, *memory, 0);
//
//    return (vk::Result)VK_SUCCESS;
//  }
//
//  // Create vertex and index buffer with given layout
//  // Note : Only does staging if a valid command buffer and transfer queue are passed
//
//  void MeshLoader::createBuffers(vk::Device device, vk::PhysicalDeviceMemoryProperties deviceMemoryProperties, MeshBuffer * meshBuffer, std::vector<VertexLayout> layout, float scale, bool useStaging, vk::CommandBuffer copyCmd, vk::Queue copyQueue)
//  {
//    std::vector<float> vertexBuffer;
//    for (int m = 0; m < mEntries.size(); m++)
//    {
//      for (int i = 0; i < mEntries[m].Vertices.size(); i++)
//      {
//        // Push vertex data depending on layout
//        for (auto& layoutDetail : layout)
//        {
//          // Position
//          if (layoutDetail == VertexLayout::VERTEX_LAYOUT_POSITION)
//          {
//            vertexBuffer.push_back(mEntries[m].Vertices[i].mPosition.x * scale);
//            vertexBuffer.push_back(mEntries[m].Vertices[i].mPosition.y * scale);
//            vertexBuffer.push_back(mEntries[m].Vertices[i].mPosition.z * scale);
//          }
//
//          // Normal
//          if (layoutDetail == VertexLayout::VERTEX_LAYOUT_NORMAL)
//          {
//            vertexBuffer.push_back(mEntries[m].Vertices[i].mNormal.x);
//            vertexBuffer.push_back(-mEntries[m].Vertices[i].mNormal.y);
//            vertexBuffer.push_back(mEntries[m].Vertices[i].mNormal.z);
//          }
//
//          // Texture coordinates
//          if (layoutDetail == VertexLayout::VERTEX_LAYOUT_UV)
//          {
//            vertexBuffer.push_back(mEntries[m].Vertices[i].mTextureCoordinates.s);
//            vertexBuffer.push_back(mEntries[m].Vertices[i].mTextureCoordinates.t);
//          }
//
//          // Color
//          if (layoutDetail == VertexLayout::VERTEX_LAYOUT_COLOR)
//          {
//            vertexBuffer.push_back(mEntries[m].Vertices[i].mColor.r);
//            vertexBuffer.push_back(mEntries[m].Vertices[i].mColor.g);
//            vertexBuffer.push_back(mEntries[m].Vertices[i].mColor.b);
//          }
//
//          // Tangent
//          if (layoutDetail == VertexLayout::VERTEX_LAYOUT_TANGENT)
//          {
//            vertexBuffer.push_back(mEntries[m].Vertices[i].mTangent.x);
//            vertexBuffer.push_back(mEntries[m].Vertices[i].mTangent.y);
//            vertexBuffer.push_back(mEntries[m].Vertices[i].mTangent.z);
//          }
//
//          // Bitangent
//          if (layoutDetail == VertexLayout::VERTEX_LAYOUT_BITANGENT)
//          {
//            vertexBuffer.push_back(mEntries[m].Vertices[i].mBinormal.x);
//            vertexBuffer.push_back(mEntries[m].Vertices[i].mBinormal.y);
//            vertexBuffer.push_back(mEntries[m].Vertices[i].mBinormal.z);
//          }
//          // Dummy layout components for padding
//          if (layoutDetail == VertexLayout::VERTEX_LAYOUT_DUMMY_FLOAT)
//          {
//            vertexBuffer.push_back(0.0f);
//          }
//
//          if (layoutDetail == VertexLayout::VERTEX_LAYOUT_DUMMY_VEC4)
//          {
//            vertexBuffer.push_back(0.0f);
//            vertexBuffer.push_back(0.0f);
//            vertexBuffer.push_back(0.0f);
//            vertexBuffer.push_back(0.0f);
//          }
//        }
//      }
//    }
//    meshBuffer->mVertices.size = vertexBuffer.size() * sizeof(float);
//
//    mDimension.min *= scale;
//    mDimension.max *= scale;
//    mDimension.size *= scale;
//
//    std::vector<uint32_t> indexBuffer;
//
//    for (uint32_t m = 0; m < mEntries.size(); m++)
//    {
//      uint32_t indexBase = (uint32_t)indexBuffer.size();
//      for (uint32_t i = 0; i < mEntries[m].Indices.size(); i++)
//      {
//        indexBuffer.push_back(mEntries[m].Indices[i] + indexBase);
//      }
//    }
//
//    meshBuffer->mIndices.size = indexBuffer.size() * sizeof(uint32_t);
//
//    meshBuffer->mIndexCount = (uint32_t)indexBuffer.size();
//
//    void* data;
//
//    // Use staging buffer to move vertex and index buffer to device local memory
//    if (useStaging && copyQueue && copyCmd)
//    {
//      // Create staging buffers
//      Buffer vertexStaging, indexStaging;
//
//      // Vertex buffer
//      createBuffer(
//        device,
//        deviceMemoryProperties,
//        vk::BufferUsageFlagBits::eTransferSrc,
//        vk::MemoryPropertyFlagBits::eHostVisible,
//        meshBuffer->mVertices.size,
//        &vertexStaging.mBuffer,
//        &vertexStaging.mMemory);
//
//      data = device.mapMemory(vertexStaging.mMemory, 0, (vk::DeviceSize)VK_WHOLE_SIZE);
//      memcpy(data, vertexBuffer.data(), meshBuffer->mVertices.size);
//      device.unmapMemory(vertexStaging.mMemory);
//
//      // Index buffer
//      createBuffer(
//        device,
//        deviceMemoryProperties,
//        vk::BufferUsageFlagBits::eTransferSrc,
//        vk::MemoryPropertyFlagBits::eHostVisible,
//        meshBuffer->mIndices.size,
//        &indexStaging.mBuffer,
//        &indexStaging.mMemory);
//
//      data = device.mapMemory(indexStaging.mMemory, 0, (vk::DeviceSize)VK_WHOLE_SIZE);
//      memcpy(data, indexBuffer.data(), meshBuffer->mIndices.size);
//      device.unmapMemory(indexStaging.mMemory);
//
//      // Create device local target buffers
//      // Vertex buffer
//      createBuffer(
//        device,
//        deviceMemoryProperties,
//        vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
//        vk::MemoryPropertyFlagBits::eDeviceLocal,
//        meshBuffer->mVertices.size,
//        &meshBuffer->mVertices.mBuffer,
//        &meshBuffer->mVertices.mMemory);
//
//      // Index buffer
//      createBuffer(
//        device,
//        deviceMemoryProperties,
//        vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
//        vk::MemoryPropertyFlagBits::eDeviceLocal,
//        meshBuffer->mIndices.size,
//        &meshBuffer->mIndices.mBuffer,
//        &meshBuffer->mIndices.mMemory);
//
//      // Copy from staging buffers
//      vk::CommandBufferBeginInfo cmdBufInfo;
//      copyCmd.begin(cmdBufInfo);
//
//      vk::BufferCopy copyRegion = {};
//
//      copyRegion.size = meshBuffer->mVertices.size;
//      copyCmd.copyBuffer(vertexStaging.mBuffer, meshBuffer->mVertices.mBuffer, 1, &copyRegion);
//
//      copyRegion.size = meshBuffer->mIndices.size;
//      copyCmd.copyBuffer(indexStaging.mBuffer, meshBuffer->mIndices.mBuffer, 1, &copyRegion);
//
//      copyCmd.end();
//
//      vk::SubmitInfo submitInfo;
//      submitInfo.commandBufferCount = 1;
//      submitInfo.pCommandBuffers = &copyCmd;
//
//      copyQueue.submit(submitInfo, nullptr);
//      copyQueue.waitIdle();
//
//      device.destroyBuffer(vertexStaging.mBuffer, nullptr);
//      device.freeMemory(vertexStaging.mMemory, nullptr);
//      device.destroyBuffer(indexStaging.mBuffer, nullptr);
//      device.freeMemory(indexStaging.mMemory, nullptr);
//    }
//    else
//    {
//      // Generate vertex buffer
//      createBuffer(
//        device,
//        deviceMemoryProperties,
//        vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferSrc,
//        vk::MemoryPropertyFlagBits::eHostVisible,
//        meshBuffer->mVertices.size,
//        &meshBuffer->mVertices.mBuffer,
//        &meshBuffer->mVertices.mMemory);
//
//
//      data = device.mapMemory(meshBuffer->mVertices.mMemory, 0, (vk::DeviceSize)VK_WHOLE_SIZE);
//      memcpy(data, vertexBuffer.data(), meshBuffer->mVertices.size);
//      device.unmapMemory(meshBuffer->mVertices.mMemory);
//
//      // Generate index buffer
//      createBuffer(
//        device,
//        deviceMemoryProperties,
//        vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferSrc,
//        vk::MemoryPropertyFlagBits::eHostVisible,
//        meshBuffer->mIndices.size,
//        &meshBuffer->mIndices.mBuffer,
//        &meshBuffer->mIndices.mMemory);
//
//      data = device.mapMemory(meshBuffer->mIndices.mMemory, 0, (vk::DeviceSize)VK_WHOLE_SIZE);
//      memcpy(data, indexBuffer.data(), meshBuffer->mIndices.size);
//      device.unmapMemory(meshBuffer->mIndices.mMemory);
//    }
//  }
//
//  // Create vertex and index buffer with given layout
//
//  void MeshLoader::createVulkanBuffers(vk::Device device, vk::PhysicalDeviceMemoryProperties deviceMemoryProperties, MeshBuffer * meshBuffer, std::vector<VertexLayout> layout, float scale)
//  {
//    createBuffers(
//      device,
//      deviceMemoryProperties,
//      meshBuffer,
//      layout,
//      scale,
//      false,
//      VK_NULL_HANDLE,
//      VK_NULL_HANDLE);
//  }
//}