#include "VkInclude.h" // IWYU pragma: keep
//
class VkObject {
public:
  VkObject() = default;
  static VkObject &sInstance();
  VkObject(const VkObject &) = delete;
  void operator=(const VkObject &) = delete;

public:
  void Cleanup();
  ~VkObject();

public: // ? Setters
  void setInstance(const vk::Instance &instance) noexcept;

public: // ? Getters
  vk::Instance getInstance() const;

private:
  vk::Instance instance = VK_NULL_HANDLE;
};