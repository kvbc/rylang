#include "Infos.hpp"

#include <format>
#include <initializer_list>
#include <string_view>
#include <utility>
#include <math.h>

namespace ry {

    using Info = Infos::Info;

    Info::Info(
        Level level, std::string_view msg,
        std::size_t startLn, std::size_t startCol
    ):
        m_level(level), m_msg(msg),
        m_startLn(startLn), m_startCol(startCol),
        m_endLn(startLn), m_endCol(startCol)
    {}

    Info::Info(
        Level level, std::string_view msg,
        std::size_t startLn, std::size_t startCol,
        std::size_t endLn, std::size_t endCol
    ):
        m_level(level), m_msg(msg),
        m_startLn(startLn), m_startCol(startCol),
        m_endLn(endLn), m_endCol(endCol)
    {}

    Info::Level Info::GetLevel() const {
        return m_level;
    }

    std::string_view Info::GetMessage() const {
        return m_msg;
    }

    std::size_t Info::GetStartLineNumber() const { return m_startLn;  }
    std::size_t Info::GetStartColumn    () const { return m_startCol; }
    std::size_t Info::GetEndLineNumber  () const { return m_endLn;    }
    std::size_t Info::GetEndColumn      () const { return m_endCol;   }

    // 

    Infos::Infos(std::string_view id, std::string_view src):
        m_id(id),
        m_src(src)
    {
        m_lineStartIndices.push_back(0);
        for(std::size_t i = 0; i < m_src.length();) {
            char c1 = m_src.at(i);
            char c2 = (i + 1 < m_src.length()) ? m_src.at(i + 1) : '\0';
            if(c1 == '\n' || c1 == '\r') {
                m_lineEndIndices.push_back(i);
                i++;
                if(c1 == '\r' && c2 == '\n')
                    i++;
                m_lineStartIndices.push_back(i);
            }
            else {
                if(c2 == '\0')
                    m_lineEndIndices.push_back(i + 1);
                i++;
            }
        }
    }

    void Infos::Push(const Info& info) {
        m_infos.push_back(info);
    }

    std::size_t Infos::GetLineStartIndex(std::size_t ln) const {
        return m_lineStartIndices.at(ln - 1);
    }

    std::size_t Infos::GetLineEndIndex(std::size_t ln) const {
        return m_lineEndIndices.at(ln - 1);
    }

    std::string Infos::Stringify() const {  
        std::string str;
        for(const Info& info : m_infos) {
            std::size_t startLn = info.GetStartLineNumber();
            std::size_t startCol = info.GetStartColumn();
            std::size_t endLn = info.GetEndLineNumber();
            std::size_t endCol = info.GetEndColumn();

            auto startPtr = m_src.data() + m_lineStartIndices[startLn - 1];
            auto endPtr = m_src.data() + m_lineEndIndices[startLn - 1];
            std::string_view line(startPtr, endPtr);
            
            std::size_t lnLen = std::floor(std::log10(startLn)) + 1;
            std::size_t leftBarLen = std::max(lnLen, m_id.length());

            std::string underLine =
                std::string(startCol - 1, ' ')
                + '^'
                + std::string(endCol - startCol, '~');
            std::string msg =
                std::string(startCol - 1, ' ')
                + std::string(info.GetMessage());

            str += std::format(
                " {:>{}} |\n {:>{}} | {}\n {:>{}} | {}\n {:>{}} | {}\n",
                m_id, leftBarLen,
                startLn, leftBarLen, line,
                "", leftBarLen, underLine,
                "", leftBarLen, msg
            );
        }
        return str;
    }

}