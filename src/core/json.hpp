export module core.json;
export import std.compat;
export import core.sys;

export extern "C++" {

enum class JsonType { Null, Bool, Number, String, Array, Object };

struct JsonValue {
  JsonType type = JsonType::Null;
  bool b_val = false;
  double num_val = 0.0;
  std::string str_val;
  std::vector<JsonValue> arr_val;
  std::vector<std::pair<std::string, JsonValue>> obj_val;

  JsonValue() = default;
  JsonValue(bool b) : type(JsonType::Bool), b_val(b) {}
  JsonValue(int n) : type(JsonType::Number), num_val(n) {}
  JsonValue(unsigned int n) : type(JsonType::Number), num_val(n) {}
  JsonValue(int64_t n) : type(JsonType::Number), num_val((double)n) {}
  JsonValue(uint64_t n) : type(JsonType::Number), num_val((double)n) {}
  JsonValue(float n) : type(JsonType::Number), num_val(n) {}
  JsonValue(double n) : type(JsonType::Number), num_val(n) {}
  JsonValue(const char* s) : type(JsonType::String), str_val(s ? s : "") {}
  JsonValue(const std::string& s) : type(JsonType::String), str_val(s) {}
  JsonValue(std::string_view s) : type(JsonType::String), str_val(s) {}

  bool is_null() const { return type == JsonType::Null; }
  bool is_bool() const { return type == JsonType::Bool; }
  bool is_number() const { return type == JsonType::Number; }
  bool is_string() const { return type == JsonType::String; }
  bool is_array() const { return type == JsonType::Array; }
  bool is_object() const { return type == JsonType::Object; }

  bool as_bool(bool def = false) const {
    if (type == JsonType::Bool) return b_val;
    if (type == JsonType::Number) return num_val != 0.0;
    return def;
  }

  int as_int(int def = 0) const {
    if (type == JsonType::Number) return (int)num_val;
    if (type == JsonType::Bool) return b_val ? 1 : 0;
    return def;
  }

  float as_float(float def = 0.0f) const {
    if (type == JsonType::Number) return (float)num_val;
    return def;
  }

  double as_double(double def = 0.0) const {
    if (type == JsonType::Number) return num_val;
    return def;
  }

  std::string as_string(const std::string& def = "") const {
    if (type == JsonType::String) return str_val;
    return def;
  }

  bool has(std::string_view key) const {
    if (type != JsonType::Object) return false;
    for (const auto& kv : obj_val) {
      if (kv.first == key) return true;
    }
    return false;
  }

  JsonValue& operator[](std::string_view key) {
    if (type != JsonType::Object) {
      type = JsonType::Object;
      obj_val.clear();
    }
    for (auto& kv : obj_val) {
      if (kv.first == key) return kv.second;
    }
    obj_val.emplace_back(std::string(key), JsonValue());
    return obj_val.back().second;
  }

  const JsonValue& operator[](std::string_view key) const {
    static const JsonValue null_v;
    if (type != JsonType::Object) return null_v;
    for (const auto& kv : obj_val) {
      if (kv.first == key) return kv.second;
    }
    return null_v;
  }

  void push_back(JsonValue v) {
    if (type != JsonType::Array) {
      type = JsonType::Array;
      arr_val.clear();
    }
    arr_val.push_back(std::move(v));
  }

  size_t size() const {
    if (type == JsonType::Array) return arr_val.size();
    if (type == JsonType::Object) return obj_val.size();
    return 0;
  }

  std::string dump(int indent = 2, int current_indent = 0) const {
    std::string out;
    std::string ind_str(indent > 0 ? current_indent : 0, ' ');
    std::string sub_ind(indent > 0 ? current_indent + indent : 0, ' ');

    switch (type) {
    case JsonType::Null:
      return "null";
    case JsonType::Bool:
      return b_val ? "true" : "false";
    case JsonType::Number: {
      char buf[64];
      if (num_val == (double)(int64_t)num_val) {
        std::snprintf(buf, sizeof(buf), "%lld", (long long)(int64_t)num_val);
      } else {
        std::snprintf(buf, sizeof(buf), "%.4f", num_val);
        char* dot = std::strchr(buf, '.');
        if (dot) {
          char* end = buf + std::strlen(buf) - 1;
          while (end > dot + 1 && *end == '0') {
            *end-- = '\0';
          }
        }
      }
      return buf;
    }
    case JsonType::String: {
      out += '\"';
      for (char c : str_val) {
        switch (c) {
        case '\"': out += "\\\""; break;
        case '\\': out += "\\\\"; break;
        case '\b': out += "\\b"; break;
        case '\f': out += "\\f"; break;
        case '\n': out += "\\n"; break;
        case '\r': out += "\\r"; break;
        case '\t': out += "\\t"; break;
        default:
          if ((unsigned char)c < 0x20) {
            char ubuf[8];
            std::snprintf(ubuf, sizeof(ubuf), "\\u%04x", (unsigned char)c);
            out += ubuf;
          } else {
            out += c;
          }
        }
      }
      out += '\"';
      return out;
    }
    case JsonType::Array: {
      if (arr_val.empty()) return "[]";
      out += "[\n";
      for (size_t i = 0; i < arr_val.size(); ++i) {
        out += sub_ind + arr_val[i].dump(indent, current_indent + indent);
        if (i + 1 < arr_val.size()) out += ",";
        out += "\n";
      }
      out += ind_str + "]";
      return out;
    }
    case JsonType::Object: {
      if (obj_val.empty()) return "{}";
      out += "{\n";
      for (size_t i = 0; i < obj_val.size(); ++i) {
        out += sub_ind + "\"" + obj_val[i].first + "\": " + obj_val[i].second.dump(indent, current_indent + indent);
        if (i + 1 < obj_val.size()) out += ",";
        out += "\n";
      }
      out += ind_str + "}";
      return out;
    }
    }
    return "null";
  }

  static JsonValue parse(std::string_view src, std::string* error_out = nullptr) {
    const char* p = src.data();
    const char* end = p + src.size();

    auto skip_whitespace = [&]() {
      while (p < end) {
        if (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') {
          ++p;
        } else if (*p == '/' && p + 1 < end && *(p + 1) == '/') {
          p += 2;
          while (p < end && *p != '\n') ++p;
        } else if (*p == '/' && p + 1 < end && *(p + 1) == '*') {
          p += 2;
          while (p + 1 < end && !(*p == '*' && *(p + 1) == '/')) ++p;
          if (p + 1 < end) p += 2;
        } else {
          break;
        }
      }
    };

    auto parse_string = [&]() -> std::string {
      std::string s;
      if (p >= end || *p != '\"') return s;
      ++p;
      while (p < end) {
        char c = *p++;
        if (c == '\"') return s;
        if (c == '\\' && p < end) {
          char esc = *p++;
          switch (esc) {
          case '\"': s += '\"'; break;
          case '\\': s += '\\'; break;
          case '/': s += '/'; break;
          case 'b': s += '\b'; break;
          case 'f': s += '\f'; break;
          case 'n': s += '\n'; break;
          case 'r': s += '\r'; break;
          case 't': s += '\t'; break;
          case 'u': {
            if (p + 4 <= end) {
              unsigned int hex = 0;
              for (int i = 0; i < 4; ++i) {
                char h = *p++;
                hex = hex * 16 + ((h >= '0' && h <= '9') ? (h - '0') :
                                  (h >= 'a' && h <= 'f') ? (h - 'a' + 10) :
                                  (h >= 'A' && h <= 'F') ? (h - 'A' + 10) : 0);
              }
              if (hex < 0x80) s += (char)hex;
              else if (hex < 0x800) { s += (char)(0xC0 | (hex >> 6)); s += (char)(0x80 | (hex & 0x3F)); }
              else { s += (char)(0xE0 | (hex >> 12)); s += (char)(0x80 | ((hex >> 6) & 0x3F)); s += (char)(0x80 | (hex & 0x3F)); }
            }
            break;
          }
          default: s += esc; break;
          }
        } else {
          s += c;
        }
      }
      return s;
    };

    std::function<JsonValue()> parse_value = [&]() -> JsonValue {
      skip_whitespace();
      if (p >= end) return JsonValue();

      if (*p == '{') {
        ++p;
        JsonValue obj;
        obj.type = JsonType::Object;
        while (p < end) {
          skip_whitespace();
          if (p < end && *p == '}') { ++p; break; }
          if (p >= end || *p != '\"') break;
          std::string key = parse_string();
          skip_whitespace();
          if (p < end && *p == ':') ++p;
          JsonValue val = parse_value();
          obj.obj_val.emplace_back(std::move(key), std::move(val));
          skip_whitespace();
          if (p < end && *p == ',') { ++p; continue; }
          if (p < end && *p == '}') { ++p; break; }
        }
        return obj;
      }

      if (*p == '[') {
        ++p;
        JsonValue arr;
        arr.type = JsonType::Array;
        while (p < end) {
          skip_whitespace();
          if (p < end && *p == ']') { ++p; break; }
          arr.arr_val.push_back(parse_value());
          skip_whitespace();
          if (p < end && *p == ',') { ++p; continue; }
          if (p < end && *p == ']') { ++p; break; }
        }
        return arr;
      }

      if (*p == '\"') {
        JsonValue s;
        s.type = JsonType::String;
        s.str_val = parse_string();
        return s;
      }

      if (*p == 't' && p + 4 <= end && std::string_view(p, 4) == "true") {
        p += 4;
        return JsonValue(true);
      }
      if (*p == 'f' && p + 5 <= end && std::string_view(p, 5) == "false") {
        p += 5;
        return JsonValue(false);
      }
      if (*p == 'n' && p + 4 <= end && std::string_view(p, 4) == "null") {
        p += 4;
        return JsonValue();
      }

      // Number
      const char* num_start = p;
      if (*p == '-') ++p;
      while (p < end && ((*p >= '0' && *p <= '9') || *p == '.' || *p == 'e' || *p == 'E' || *p == '+' || *p == '-')) {
        ++p;
      }
      double val = 0.0;
      std::from_chars(num_start, p, val);
      return JsonValue(val);
    };

    return parse_value();
  }
};

}
