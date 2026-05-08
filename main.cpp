#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include "Proyecto_v006.cpp"

int main() {
    const int cols = 12;
    const int rows = 20;
    const int cellW = 70;
    const int cellH = 32;
    const int margin = 24;
    const int rowHeaderW = 44;
    const int colHeaderH = 28;

    const int gridX = margin + rowHeaderW;
    const int gridY = margin + colHeaderH;

    sf::RenderWindow window(
        sf::VideoMode({
            static_cast<unsigned int>(gridX + cols * cellW + margin),
            static_cast<unsigned int>(gridY + rows * cellH + margin)
        }),
        "MiniExcel"
    );

    SparseMatrix cells;
    int selectedRow = 0;
    int selectedCol = 0;
    std::string editBuffer;

    sf::Font font;
    const bool fontLoaded =
        font.openFromFile("C:/Windows/Fonts/segoeui.ttf") ||
        font.openFromFile("C:/Windows/Fonts/arial.ttf");

    auto syncBufferFromCell = [&]() {
        int value = 0;
        if (cells.GetCellValue(selectedRow + 1, selectedCol + 1, value)) {
            editBuffer = std::to_string(value);
        } else {
            editBuffer.clear();
        }
    };

    auto updateTitle = [&]() {
        const std::string title =
            "MiniExcel | celda [" + std::to_string(selectedRow + 1) + "," + std::to_string(selectedCol + 1) +
            "] | valor: " + (editBuffer.empty() ? std::string("(vacio)") : editBuffer) +
            " | Enter=guardar, Delete=borrar";
        window.setTitle(title);
    };
    syncBufferFromCell();
    updateTitle();

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                const auto key = keyPressed->code;

                if (key == sf::Keyboard::Key::Left) {
                    if (selectedCol > 0) {
                        selectedCol--;
                        syncBufferFromCell();
                    }
                } else if (key == sf::Keyboard::Key::Right) {
                    if (selectedCol < cols - 1) {
                        selectedCol++;
                        syncBufferFromCell();
                    }
                } else if (key == sf::Keyboard::Key::Up) {
                    if (selectedRow > 0) {
                        selectedRow--;
                        syncBufferFromCell();
                    }
                } else if (key == sf::Keyboard::Key::Down) {
                    if (selectedRow < rows - 1) {
                        selectedRow++;
                        syncBufferFromCell();
                    }
                } else if (key == sf::Keyboard::Key::Enter) {
                    if (editBuffer.empty() || editBuffer == "-") {
                        cells.ClearCellValue(selectedRow + 1, selectedCol + 1);
                    } else {
                        cells.SetCellValue(selectedRow + 1, selectedCol + 1, std::stoi(editBuffer));
                    }
                } else if (key == sf::Keyboard::Key::Backspace || key == sf::Keyboard::Key::Delete) {
                    cells.ClearCellValue(selectedRow + 1, selectedCol + 1);
                    editBuffer.clear();
                }

                updateTitle();
            }

            if (const auto* textEntered = event->getIf<sf::Event::TextEntered>()) {
                const char32_t cp = textEntered->unicode;
                if (cp >= U'0' && cp <= U'9') {
                    if (editBuffer.size() < 9) {
                        editBuffer.push_back(static_cast<char>(cp));
                    }
                } else if (cp == U'-') {
                    if (editBuffer.empty()) {
                        editBuffer.push_back('-');
                    }
                }
                updateTitle();
            }

            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePressed->button == sf::Mouse::Button::Left) {
                    const int mx = mousePressed->position.x;
                    const int my = mousePressed->position.y;
                    const int gx = mx - gridX;
                    const int gy = my - gridY;
                    if (gx >= 0 && gy >= 0) {
                        const int c = gx / cellW;
                        const int r = gy / cellH;
                        if (c >= 0 && c < cols && r >= 0 && r < rows) {
                            selectedCol = c;
                            selectedRow = r;
                            syncBufferFromCell();
                            updateTitle();
                        }
                    }
                }
            }
        }

        window.clear(sf::Color(20, 24, 30));

        if (fontLoaded) {
            sf::RectangleShape corner(sf::Vector2f(static_cast<float>(rowHeaderW - 1), static_cast<float>(colHeaderH - 1)));
            corner.setPosition(sf::Vector2f(static_cast<float>(margin), static_cast<float>(margin)));
            corner.setFillColor(sf::Color(55, 60, 72));
            window.draw(corner);

            for (int c = 0; c < cols; ++c) {
                sf::RectangleShape headerCell(sf::Vector2f(static_cast<float>(cellW - 1), static_cast<float>(colHeaderH - 1)));
                headerCell.setPosition(sf::Vector2f(static_cast<float>(gridX + c * cellW), static_cast<float>(margin)));
                headerCell.setFillColor(sf::Color(55, 60, 72));
                window.draw(headerCell);

                char labelChar = static_cast<char>('A' + c);
                std::string label(1, labelChar);
                sf::Text text(font, label, 14);
                text.setFillColor(sf::Color::White);
                text.setPosition(sf::Vector2f(
                    static_cast<float>(gridX + c * cellW + (cellW / 2) - 5),
                    static_cast<float>(margin + 5)
                ));
                window.draw(text);
            }

            for (int r = 0; r < rows; ++r) {
                sf::RectangleShape headerCell(sf::Vector2f(static_cast<float>(rowHeaderW - 1), static_cast<float>(cellH - 1)));
                headerCell.setPosition(sf::Vector2f(static_cast<float>(margin), static_cast<float>(gridY + r * cellH)));
                headerCell.setFillColor(sf::Color(55, 60, 72));
                window.draw(headerCell);

                sf::Text text(font, std::to_string(r + 1), 14);
                text.setFillColor(sf::Color::White);
                text.setPosition(sf::Vector2f(
                    static_cast<float>(margin + 10),
                    static_cast<float>(gridY + r * cellH + 6)
                ));
                window.draw(text);
            }
        }

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                sf::RectangleShape cell(sf::Vector2f(static_cast<float>(cellW - 1), static_cast<float>(cellH - 1)));
                cell.setPosition(sf::Vector2f(static_cast<float>(gridX + c * cellW), static_cast<float>(gridY + r * cellH)));

                const bool selected = (r == selectedRow && c == selectedCol);
                int cellValue = 0;
                const bool hasValue = cells.GetCellValue(r + 1, c + 1, cellValue);

                if (selected) {
                    cell.setFillColor(sf::Color(70, 120, 220));
                } else if (hasValue) {
                    cell.setFillColor(sf::Color(70, 150, 90));
                } else {
                    cell.setFillColor(sf::Color(40, 45, 55));
                }

                window.draw(cell);

                if (fontLoaded) {
                    std::string textValue;
                    if (selected && !editBuffer.empty()) {
                        textValue = editBuffer;
                    } else if (hasValue) {
                        textValue = std::to_string(cellValue);
                    }

                    if (!textValue.empty()) {
                        sf::Text text(font, textValue, 14);
                        text.setFillColor(sf::Color::White);
                        text.setPosition(sf::Vector2f(
                            static_cast<float>(gridX + c * cellW + 6),
                            static_cast<float>(gridY + r * cellH + 6)
                        ));
                        window.draw(text);
                    }
                }
            }
        }

        window.display();
    }

    return 0;
}
