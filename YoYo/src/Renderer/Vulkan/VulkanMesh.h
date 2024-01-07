#include "Renderer/Mesh.h"

#include "VulkanStructures.h"

namespace yoyo
{
    // Returns the attribute descriptions of the current vertex format
    const std::vector<VkVertexInputAttributeDescription>& VertexAttributeDescriptions();

    // Returns the binding descriptions of the current vertex format
    const std::vector<VkVertexInputBindingDescription>& VertexBindingDescriptions();

    class VulkanMesh : public Mesh
    {
    public:
        VulkanMesh();
        virtual ~VulkanMesh();
    private:
        AllocatedBuffer m_buffer;
    };
};