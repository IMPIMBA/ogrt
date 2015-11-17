/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: protocol/ogrt.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "ogrt.pb-c.h"
void   ogrt__job_start__init
                     (OGRT__JobStart         *message)
{
  static OGRT__JobStart init_value = OGRT__JOB_START__INIT;
  *message = init_value;
}
size_t ogrt__job_start__get_packed_size
                     (const OGRT__JobStart *message)
{
  assert(message->base.descriptor == &ogrt__job_start__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t ogrt__job_start__pack
                     (const OGRT__JobStart *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &ogrt__job_start__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t ogrt__job_start__pack_to_buffer
                     (const OGRT__JobStart *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &ogrt__job_start__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
OGRT__JobStart *
       ogrt__job_start__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (OGRT__JobStart *)
     protobuf_c_message_unpack (&ogrt__job_start__descriptor,
                                allocator, len, data);
}
void   ogrt__job_start__free_unpacked
                     (OGRT__JobStart *message,
                      ProtobufCAllocator *allocator)
{
  assert(message->base.descriptor == &ogrt__job_start__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   ogrt__job_end__init
                     (OGRT__JobEnd         *message)
{
  static OGRT__JobEnd init_value = OGRT__JOB_END__INIT;
  *message = init_value;
}
size_t ogrt__job_end__get_packed_size
                     (const OGRT__JobEnd *message)
{
  assert(message->base.descriptor == &ogrt__job_end__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t ogrt__job_end__pack
                     (const OGRT__JobEnd *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &ogrt__job_end__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t ogrt__job_end__pack_to_buffer
                     (const OGRT__JobEnd *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &ogrt__job_end__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
OGRT__JobEnd *
       ogrt__job_end__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (OGRT__JobEnd *)
     protobuf_c_message_unpack (&ogrt__job_end__descriptor,
                                allocator, len, data);
}
void   ogrt__job_end__free_unpacked
                     (OGRT__JobEnd *message,
                      ProtobufCAllocator *allocator)
{
  assert(message->base.descriptor == &ogrt__job_end__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   ogrt__shared_object__init
                     (OGRT__SharedObject         *message)
{
  static OGRT__SharedObject init_value = OGRT__SHARED_OBJECT__INIT;
  *message = init_value;
}
size_t ogrt__shared_object__get_packed_size
                     (const OGRT__SharedObject *message)
{
  assert(message->base.descriptor == &ogrt__shared_object__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t ogrt__shared_object__pack
                     (const OGRT__SharedObject *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &ogrt__shared_object__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t ogrt__shared_object__pack_to_buffer
                     (const OGRT__SharedObject *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &ogrt__shared_object__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
OGRT__SharedObject *
       ogrt__shared_object__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (OGRT__SharedObject *)
     protobuf_c_message_unpack (&ogrt__shared_object__descriptor,
                                allocator, len, data);
}
void   ogrt__shared_object__free_unpacked
                     (OGRT__SharedObject *message,
                      ProtobufCAllocator *allocator)
{
  assert(message->base.descriptor == &ogrt__shared_object__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   ogrt__process_info__init
                     (OGRT__ProcessInfo         *message)
{
  static OGRT__ProcessInfo init_value = OGRT__PROCESS_INFO__INIT;
  *message = init_value;
}
size_t ogrt__process_info__get_packed_size
                     (const OGRT__ProcessInfo *message)
{
  assert(message->base.descriptor == &ogrt__process_info__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t ogrt__process_info__pack
                     (const OGRT__ProcessInfo *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &ogrt__process_info__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t ogrt__process_info__pack_to_buffer
                     (const OGRT__ProcessInfo *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &ogrt__process_info__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
OGRT__ProcessInfo *
       ogrt__process_info__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (OGRT__ProcessInfo *)
     protobuf_c_message_unpack (&ogrt__process_info__descriptor,
                                allocator, len, data);
}
void   ogrt__process_info__free_unpacked
                     (OGRT__ProcessInfo *message,
                      ProtobufCAllocator *allocator)
{
  assert(message->base.descriptor == &ogrt__process_info__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   ogrt__fork__init
                     (OGRT__Fork         *message)
{
  static OGRT__Fork init_value = OGRT__FORK__INIT;
  *message = init_value;
}
size_t ogrt__fork__get_packed_size
                     (const OGRT__Fork *message)
{
  assert(message->base.descriptor == &ogrt__fork__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t ogrt__fork__pack
                     (const OGRT__Fork *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &ogrt__fork__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t ogrt__fork__pack_to_buffer
                     (const OGRT__Fork *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &ogrt__fork__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
OGRT__Fork *
       ogrt__fork__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (OGRT__Fork *)
     protobuf_c_message_unpack (&ogrt__fork__descriptor,
                                allocator, len, data);
}
void   ogrt__fork__free_unpacked
                     (OGRT__Fork *message,
                      ProtobufCAllocator *allocator)
{
  assert(message->base.descriptor == &ogrt__fork__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   ogrt__execve__init
                     (OGRT__Execve         *message)
{
  static OGRT__Execve init_value = OGRT__EXECVE__INIT;
  *message = init_value;
}
size_t ogrt__execve__get_packed_size
                     (const OGRT__Execve *message)
{
  assert(message->base.descriptor == &ogrt__execve__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t ogrt__execve__pack
                     (const OGRT__Execve *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &ogrt__execve__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t ogrt__execve__pack_to_buffer
                     (const OGRT__Execve *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &ogrt__execve__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
OGRT__Execve *
       ogrt__execve__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (OGRT__Execve *)
     protobuf_c_message_unpack (&ogrt__execve__descriptor,
                                allocator, len, data);
}
void   ogrt__execve__free_unpacked
                     (OGRT__Execve *message,
                      ProtobufCAllocator *allocator)
{
  assert(message->base.descriptor == &ogrt__execve__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor ogrt__job_start__field_descriptors[2] =
{
  {
    "jobid",
    100,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(OGRT__JobStart, jobid),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "start_time",
    101,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT64,
    0,   /* quantifier_offset */
    offsetof(OGRT__JobStart, start_time),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned ogrt__job_start__field_indices_by_name[] = {
  0,   /* field[0] = jobid */
  1,   /* field[1] = start_time */
};
static const ProtobufCIntRange ogrt__job_start__number_ranges[1 + 1] =
{
  { 100, 0 },
  { 0, 2 }
};
const ProtobufCMessageDescriptor ogrt__job_start__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "OGRT.JobStart",
  "JobStart",
  "OGRT__JobStart",
  "OGRT",
  sizeof(OGRT__JobStart),
  2,
  ogrt__job_start__field_descriptors,
  ogrt__job_start__field_indices_by_name,
  1,  ogrt__job_start__number_ranges,
  (ProtobufCMessageInit) ogrt__job_start__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor ogrt__job_end__field_descriptors[2] =
{
  {
    "jobid",
    200,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(OGRT__JobEnd, jobid),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "end_time",
    201,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT64,
    0,   /* quantifier_offset */
    offsetof(OGRT__JobEnd, end_time),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned ogrt__job_end__field_indices_by_name[] = {
  1,   /* field[1] = end_time */
  0,   /* field[0] = jobid */
};
static const ProtobufCIntRange ogrt__job_end__number_ranges[1 + 1] =
{
  { 200, 0 },
  { 0, 2 }
};
const ProtobufCMessageDescriptor ogrt__job_end__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "OGRT.JobEnd",
  "JobEnd",
  "OGRT__JobEnd",
  "OGRT",
  sizeof(OGRT__JobEnd),
  2,
  ogrt__job_end__field_descriptors,
  ogrt__job_end__field_indices_by_name,
  1,  ogrt__job_end__number_ranges,
  (ProtobufCMessageInit) ogrt__job_end__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor ogrt__shared_object__field_descriptors[2] =
{
  {
    "path",
    400,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(OGRT__SharedObject, path),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "signature",
    401,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(OGRT__SharedObject, signature),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned ogrt__shared_object__field_indices_by_name[] = {
  0,   /* field[0] = path */
  1,   /* field[1] = signature */
};
static const ProtobufCIntRange ogrt__shared_object__number_ranges[1 + 1] =
{
  { 400, 0 },
  { 0, 2 }
};
const ProtobufCMessageDescriptor ogrt__shared_object__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "OGRT.SharedObject",
  "SharedObject",
  "OGRT__SharedObject",
  "OGRT",
  sizeof(OGRT__SharedObject),
  2,
  ogrt__shared_object__field_descriptors,
  ogrt__shared_object__field_indices_by_name,
  1,  ogrt__shared_object__number_ranges,
  (ProtobufCMessageInit) ogrt__shared_object__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor ogrt__process_info__field_descriptors[5] =
{
  {
    "pid",
    300,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    offsetof(OGRT__ProcessInfo, pid),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "parent_pid",
    301,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    offsetof(OGRT__ProcessInfo, parent_pid),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "time",
    302,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT64,
    0,   /* quantifier_offset */
    offsetof(OGRT__ProcessInfo, time),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "environment_variables",
    303,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_STRING,
    offsetof(OGRT__ProcessInfo, n_environment_variables),
    offsetof(OGRT__ProcessInfo, environment_variables),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "shared_object",
    304,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_MESSAGE,
    offsetof(OGRT__ProcessInfo, n_shared_object),
    offsetof(OGRT__ProcessInfo, shared_object),
    &ogrt__shared_object__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned ogrt__process_info__field_indices_by_name[] = {
  3,   /* field[3] = environment_variables */
  1,   /* field[1] = parent_pid */
  0,   /* field[0] = pid */
  4,   /* field[4] = shared_object */
  2,   /* field[2] = time */
};
static const ProtobufCIntRange ogrt__process_info__number_ranges[1 + 1] =
{
  { 300, 0 },
  { 0, 5 }
};
const ProtobufCMessageDescriptor ogrt__process_info__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "OGRT.ProcessInfo",
  "ProcessInfo",
  "OGRT__ProcessInfo",
  "OGRT",
  sizeof(OGRT__ProcessInfo),
  5,
  ogrt__process_info__field_descriptors,
  ogrt__process_info__field_indices_by_name,
  1,  ogrt__process_info__number_ranges,
  (ProtobufCMessageInit) ogrt__process_info__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor ogrt__fork__field_descriptors[4] =
{
  {
    "hostname",
    500,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(OGRT__Fork, hostname),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "parent_pid",
    501,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    offsetof(OGRT__Fork, parent_pid),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "child_pid",
    502,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    offsetof(OGRT__Fork, child_pid),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "name",
    503,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(OGRT__Fork, name),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned ogrt__fork__field_indices_by_name[] = {
  2,   /* field[2] = child_pid */
  0,   /* field[0] = hostname */
  3,   /* field[3] = name */
  1,   /* field[1] = parent_pid */
};
static const ProtobufCIntRange ogrt__fork__number_ranges[1 + 1] =
{
  { 500, 0 },
  { 0, 4 }
};
const ProtobufCMessageDescriptor ogrt__fork__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "OGRT.Fork",
  "Fork",
  "OGRT__Fork",
  "OGRT",
  sizeof(OGRT__Fork),
  4,
  ogrt__fork__field_descriptors,
  ogrt__fork__field_indices_by_name,
  1,  ogrt__fork__number_ranges,
  (ProtobufCMessageInit) ogrt__fork__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor ogrt__execve__field_descriptors[7] =
{
  {
    "hostname",
    600,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(OGRT__Execve, hostname),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "pid",
    601,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    offsetof(OGRT__Execve, pid),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "parent_pid",
    602,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    offsetof(OGRT__Execve, parent_pid),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "filename",
    603,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(OGRT__Execve, filename),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "arguments",
    604,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_STRING,
    offsetof(OGRT__Execve, n_arguments),
    offsetof(OGRT__Execve, arguments),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "environment_variables",
    605,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_STRING,
    offsetof(OGRT__Execve, n_environment_variables),
    offsetof(OGRT__Execve, environment_variables),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "uuid",
    606,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(OGRT__Execve, uuid),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned ogrt__execve__field_indices_by_name[] = {
  4,   /* field[4] = arguments */
  5,   /* field[5] = environment_variables */
  3,   /* field[3] = filename */
  0,   /* field[0] = hostname */
  2,   /* field[2] = parent_pid */
  1,   /* field[1] = pid */
  6,   /* field[6] = uuid */
};
static const ProtobufCIntRange ogrt__execve__number_ranges[1 + 1] =
{
  { 600, 0 },
  { 0, 7 }
};
const ProtobufCMessageDescriptor ogrt__execve__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "OGRT.Execve",
  "Execve",
  "OGRT__Execve",
  "OGRT",
  sizeof(OGRT__Execve),
  7,
  ogrt__execve__field_descriptors,
  ogrt__execve__field_indices_by_name,
  1,  ogrt__execve__number_ranges,
  (ProtobufCMessageInit) ogrt__execve__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCEnumValue ogrt__message_type__enum_values_by_number[6] =
{
  { "JobStartMsg", "OGRT__MESSAGE_TYPE__JobStartMsg", 10 },
  { "JobEndMsg", "OGRT__MESSAGE_TYPE__JobEndMsg", 11 },
  { "ProcessInfoMsg", "OGRT__MESSAGE_TYPE__ProcessInfoMsg", 12 },
  { "SharedObjectMsg", "OGRT__MESSAGE_TYPE__SharedObjectMsg", 13 },
  { "ForkMsg", "OGRT__MESSAGE_TYPE__ForkMsg", 14 },
  { "ExecveMsg", "OGRT__MESSAGE_TYPE__ExecveMsg", 15 },
};
static const ProtobufCIntRange ogrt__message_type__value_ranges[] = {
{10, 0},{0, 6}
};
static const ProtobufCEnumValueIndex ogrt__message_type__enum_values_by_name[6] =
{
  { "ExecveMsg", 5 },
  { "ForkMsg", 4 },
  { "JobEndMsg", 1 },
  { "JobStartMsg", 0 },
  { "ProcessInfoMsg", 2 },
  { "SharedObjectMsg", 3 },
};
const ProtobufCEnumDescriptor ogrt__message_type__descriptor =
{
  PROTOBUF_C__ENUM_DESCRIPTOR_MAGIC,
  "OGRT.MessageType",
  "MessageType",
  "OGRT__MessageType",
  "OGRT",
  6,
  ogrt__message_type__enum_values_by_number,
  6,
  ogrt__message_type__enum_values_by_name,
  1,
  ogrt__message_type__value_ranges,
  NULL,NULL,NULL,NULL   /* reserved[1234] */
};