#pragma once

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

            std::string_view GetMessage() const;
            Level GetLevel() const;

            std::size_t GetStartLineNumber() const;
            std::size_t GetStartColumn() const;
            std::size_t GetEndLineNumber() const;
            std::size_t GetEndColumn() const;

        private:
            std::size_t m_startLn;
            std::size_t m_startCol;
            std::size_t m_endLn;
            std::size_t m_endCol;
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
