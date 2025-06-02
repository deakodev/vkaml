#include "utils.h"

#include <echo.h>
#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>

#include <vulkan/vulkan.h>

CAML_WRAP_HANDLE_OPT(VkInstance, VkInstance)

CAMLprim value ml_vulkan_init(value unit)
{
    CAMLparam1(unit);

    VkApplicationInfo appInfo = { .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO };
    appInfo.pApplicationName  = "deako_app";
    appInfo.pEngineName       = "deako_engine";
    appInfo.apiVersion        = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = { .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    createInfo.pApplicationInfo     = &appInfo;

    VkInstance instance;
    VK_CHECK(vkCreateInstance(&createInfo, NULL, &instance));

    CAMLreturn(Val_VkInstance(instance));
}
