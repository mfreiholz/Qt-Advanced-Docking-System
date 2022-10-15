#include <string>

#include "AdvancedDockingSystem/AdvancedDockingSystem.hpp"

exported_class::exported_class()
    : m_name {"AdvancedDockingSystem"}
{
}

auto exported_class::name() const -> const char*
{
  return m_name.c_str();
}
