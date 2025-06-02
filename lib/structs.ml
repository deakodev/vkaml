[%%cstruct
type vk_application_info = {
  s_type : uint32_t;
  p_next : uint64_t;
  p_application_name : uint64_t;
  application_version : uint32_t;
  p_engine_name : uint64_t;
  engine_version : uint32_t;
  api_version : uint32_t;
}
[@@little_endian]]
