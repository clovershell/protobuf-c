// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: kenton@google.com (Kenton Varda)
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.

// Copyright (c) 2008-2025, Dave Benson and the protobuf-c authors.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Modified to implement C code by Dave Benson.

#include <google/protobuf/io/printer.h>
#include <google/protobuf/wire_format.h>

#include <protobuf-c/protobuf-c.pb.h>

#include "c_bytes_field.h"
#include "c_helpers.h"

namespace protobuf_c {

void SetBytesVariables(const google::protobuf::FieldDescriptor* descriptor,
                        std::map<std::string, std::string>* variables) {
  (*variables)["name"] = FieldName(descriptor);
  (*variables)["default"] =
    "\"" + CEscape(descriptor->default_value_string()) + "\"";
  (*variables)["deprecated"] = FieldDeprecated(descriptor);
}

// ===================================================================

BytesFieldGenerator::
BytesFieldGenerator(const google::protobuf::FieldDescriptor* descriptor)
  : FieldGenerator(descriptor) {
  SetBytesVariables(descriptor, &variables_);
  variables_["default_value"] = descriptor->has_default_value()
                              ? GetDefaultValue() 
			      : std::string("{0,NULL}");
}

BytesFieldGenerator::~BytesFieldGenerator() {}

void BytesFieldGenerator::GenerateStructMembers(google::protobuf::io::Printer* printer) const
{
  switch (descriptor_->label()) {
    case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
      printer->Print(variables_, "ProtobufCBinaryData $name$$deprecated$;\n");
      break;
    case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
      if (descriptor_->containing_oneof() == NULL && FieldSyntax(descriptor_) == 2)
        printer->Print(variables_, "protobuf_c_boolean has_$name$$deprecated$;\n");
      printer->Print(variables_, "ProtobufCBinaryData $name$$deprecated$;\n");
      break;
    case google::protobuf::FieldDescriptor::LABEL_REPEATED:
      printer->Print(variables_, "size_t n_$name$$deprecated$;\n");
      printer->Print(variables_, "ProtobufCBinaryData *$name$$deprecated$;\n");
      break;
  }
}
void BytesFieldGenerator::GenerateDefaultValueDeclarations(google::protobuf::io::Printer* printer) const
{
  std::map<std::string, std::string> vars;
  vars["default_value_data"] = FullNameToLower(descriptor_->full_name(), descriptor_->file())
	                     + "__default_value_data";
  printer->Print(vars, "extern uint8_t $default_value_data$[];\n");
}

void BytesFieldGenerator::GenerateDefaultValueImplementations(google::protobuf::io::Printer* printer) const
{
  std::map<std::string, std::string> vars;
  vars["default_value_data"] = FullNameToLower(descriptor_->full_name(), descriptor_->file())
	                     + "__default_value_data";
  vars["escaped"] = CEscape(descriptor_->default_value_string());
  printer->Print(vars, "uint8_t $default_value_data$[] = \"$escaped$\";\n");
}
std::string BytesFieldGenerator::GetDefaultValue(void) const
{
  return "{ "
	+ SimpleItoa(descriptor_->default_value_string().size())
	+ ", "
	+ FullNameToLower(descriptor_->full_name(), descriptor_->file())
	+ "__default_value_data }";
}
void BytesFieldGenerator::GenerateStaticInit(google::protobuf::io::Printer* printer) const
{
  switch (descriptor_->label()) {
    case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
      printer->Print(variables_, "$default_value$");
      break;
    case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
      if (FieldSyntax(descriptor_) == 2)
        printer->Print(variables_, "0, ");
      printer->Print(variables_, "$default_value$");
      break;
    case google::protobuf::FieldDescriptor::LABEL_REPEATED:
      // no support for default?
      printer->Print("0,NULL");
      break;
  }
}
void BytesFieldGenerator::GenerateDescriptorInitializer(google::protobuf::io::Printer* printer) const
{
  GenerateDescriptorInitializerGeneric(printer, true, "BYTES", "NULL");
}

}  // namespace protobuf_c
