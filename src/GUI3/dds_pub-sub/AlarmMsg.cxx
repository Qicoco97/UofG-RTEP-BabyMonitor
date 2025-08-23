

#ifdef _WIN32
// Remove linker warning LNK4221 on Visual Studio
namespace {
char dummy;
}  // namespace
#endif  // _WIN32

#include "AlarmMsg.h"
#include <fastcdr/Cdr.h>

#include <fastcdr/exceptions/BadParamException.h>
using namespace eprosima::fastcdr::exception;

#include <utility>

AlarmMsg::AlarmMsg()
{
    // m_index com.eprosima.idl.parser.typecode.PrimitiveTypeCode@2b552920
    m_index = 0;
    // m_message com.eprosima.idl.parser.typecode.StringTypeCode@1f36e637
    m_message ="";

}

AlarmMsg::~AlarmMsg()
{

}

AlarmMsg::AlarmMsg(
        const AlarmMsg& x)
{
    m_index = x.m_index;
    m_message = x.m_message;
}

AlarmMsg::AlarmMsg(
        AlarmMsg&& x)
{
    m_index = x.m_index;
    m_message = std::move(x.m_message);
}

AlarmMsg& AlarmMsg::operator =(
        const AlarmMsg& x)
{

    m_index = x.m_index;
    m_message = x.m_message;

    return *this;
}

AlarmMsg& AlarmMsg::operator =(
        AlarmMsg&& x)
{

    m_index = x.m_index;
    m_message = std::move(x.m_message);

    return *this;
}

bool AlarmMsg::operator ==(
        const AlarmMsg& x) const
{

    return (m_index == x.m_index && m_message == x.m_message);
}

bool AlarmMsg::operator !=(
        const AlarmMsg& x) const
{
    return !(*this == x);
}

size_t AlarmMsg::getMaxCdrSerializedSize(
        size_t current_alignment)
{
    size_t initial_alignment = current_alignment;


    current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4);


    current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4) + 255 + 1;


    return current_alignment - initial_alignment;
}

size_t AlarmMsg::getCdrSerializedSize(
        const AlarmMsg& data,
        size_t current_alignment)
{
    (void)data;
    size_t initial_alignment = current_alignment;


    current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4);


    current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4) + data.message().size() + 1;


    return current_alignment - initial_alignment;
}

void AlarmMsg::serialize(
        eprosima::fastcdr::Cdr& scdr) const
{

    scdr << m_index;
    scdr << m_message;

}

void AlarmMsg::deserialize(
        eprosima::fastcdr::Cdr& dcdr)
{

    dcdr >> m_index;
    dcdr >> m_message;
}

/*!
 * @brief This function sets a value in member index
 * @param _index New value for member index
 */
void AlarmMsg::index(
        uint32_t _index)
{
    m_index = _index;
}

/*!
 * @brief This function returns the value of member index
 * @return Value of member index
 */
uint32_t AlarmMsg::index() const
{
    return m_index;
}

/*!
 * @brief This function returns a reference to member index
 * @return Reference to member index
 */
uint32_t& AlarmMsg::index()
{
    return m_index;
}

/*!
 * @brief This function copies the value in member message
 * @param _message New value to be copied in member message
 */
void AlarmMsg::message(
        const std::string& _message)
{
    m_message = _message;
}

/*!
 * @brief This function moves the value in member message
 * @param _message New value to be moved in member message
 */
void AlarmMsg::message(
        std::string&& _message)
{
    m_message = std::move(_message);
}

/*!
 * @brief This function returns a constant reference to member message
 * @return Constant reference to member message
 */
const std::string& AlarmMsg::message() const
{
    return m_message;
}

/*!
 * @brief This function returns a reference to member message
 * @return Reference to member message
 */
std::string& AlarmMsg::message()
{
    return m_message;
}

size_t AlarmMsg::getKeyMaxCdrSerializedSize(
        size_t current_alignment)
{
    size_t current_align = current_alignment;



    return current_align;
}

bool AlarmMsg::isKeyDefined()
{
    return false;
}

void AlarmMsg::serializeKey(
        eprosima::fastcdr::Cdr& scdr) const
{
    (void) scdr;
      
}
