#pragma once

#include "SourcePosition.hpp"

#include <initializer_list>
#include <string_view>
#include <string>
#include <vector>

namespace ry {

    class Infos {
    public:
        class Info {
        public:
            enum class Level {
                INFO, WARN, ERROR
            };

            Info(
                Level lvl, std::string_view msg,
                std::size_t startLn, std::size_t startCol
            );
            Info(
                Level lvl, std::string_view msg,
                std::size_t startLn, std::size_t startCol,
                std::size_t endLn, std::size_t endCol
            );
            Info(
                Level lvl, std::string_view msg,
                const SourcePosition& srcPos
            );
            Info(
                Level lvl, std::string_view msg
            );

            std::string_view GetMessage() const;
            Level GetLevel() const;
            const SourcePosition& GetSourcePosition() const;

        private:
            SourcePosition m_srcPos;
            std::string m_msg;
            Level m_level;
        };

        Infos(std::string_view id, std::string_view src);

        void Push(const Info& info);

        std::size_t GetLineStartIndex(std::size_t ln) const;
        std::size_t GetLineEndIndex(std::size_t ln) const;

        std::string Stringify() const;

    private:
        std::string m_src;
        std::string m_id;
        std::vector<std::size_t> m_lineStartIndices;
        std::vector<std::size_t> m_lineEndIndices;
        std::vector<Info> m_infos;
    };
}
