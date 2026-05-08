#include <SFML/Graphics.hpp>

#include <algorithm>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>

// Coordenada usada como llave del unordered_map: una celda se identifica por fila y columna.
struct CellKey {
    int row;
    int col;

    bool operator==(const CellKey& other) const {
        return row == other.row && col == other.col;
    }
};

struct CellKeyHash {
    std::size_t operator()(const CellKey& key) const {
        return (static_cast<std::size_t>(key.row) << 16) ^ static_cast<std::size_t>(key.col);
    }
};

struct CellCoord {
    int row;
    int col;
};

// Operaciones que se pueden ejecutar sobre el rango seleccionado en la vista.
enum class RangeOperation {
    Sum,
    Subtract,
    Average,
    Maximum,
    Minimum
};

// Valores acumulados en un rango. Con una sola pasada se obtiene todo lo necesario.
struct OperationResult {
    bool hasValues = false;
    int sum = 0;
    int difference = 0;
    int minValue = 0;
    int maxValue = 0;
    int count = 0;
};

// Convierte un indice de columna a formato tipo hoja de calculo: 0 -> A, 1 -> B.
std::string columnLabel(int col) {
    std::string label;
    int value = col;

    do {
        label.insert(label.begin(), static_cast<char>('A' + (value % 26)));
        value = (value / 26) - 1;
    } while (value >= 0);

    return label;
}

std::string cellLabel(int row, int col) {
    return columnLabel(col) + std::to_string(row + 1);
}

// Devuelve el texto del rango seleccionado, por ejemplo A1:C4.
std::string rangeLabel(CellCoord a, CellCoord b) {
    const int rowStart = std::min(a.row, b.row);
    const int rowEnd = std::max(a.row, b.row);
    const int colStart = std::min(a.col, b.col);
    const int colEnd = std::max(a.col, b.col);

    if (rowStart == rowEnd && colStart == colEnd) {
        return cellLabel(rowStart, colStart);
    }

    return cellLabel(rowStart, colStart) + ":" + cellLabel(rowEnd, colEnd);
}

bool isInsideRange(int row, int col, CellCoord a, CellCoord b) {
    const int rowStart = std::min(a.row, b.row);
    const int rowEnd = std::max(a.row, b.row);
    const int colStart = std::min(a.col, b.col);
    const int colEnd = std::max(a.col, b.col);

    return row >= rowStart && row <= rowEnd && col >= colStart && col <= colEnd;
}

// Muestra promedios como 4.5 en lugar de 4.500000.
std::string formatDecimal(double value) {
    std::ostringstream out;
    out.precision(2);
    out << std::fixed << value;

    std::string text = out.str();
    while (text.size() > 1 && text.back() == '0') {
        text.pop_back();
    }
    if (!text.empty() && text.back() == '.') {
        text.pop_back();
    }

    return text;
}

std::string operationName(RangeOperation operation) {
    switch (operation) {
        case RangeOperation::Sum:
            return "SUMA";
        case RangeOperation::Subtract:
            return "RESTA";
        case RangeOperation::Average:
            return "PROMEDIO";
        case RangeOperation::Maximum:
            return "MAXIMO";
        case RangeOperation::Minimum:
            return "MINIMO";
    }

    return "OPERACION";
}

std::string operationValue(RangeOperation operation, const OperationResult& result) {
    switch (operation) {
        case RangeOperation::Sum:
            return std::to_string(result.sum);
        case RangeOperation::Subtract:
            return std::to_string(result.difference);
        case RangeOperation::Average:
            return formatDecimal(static_cast<double>(result.sum) / result.count);
        case RangeOperation::Maximum:
            return std::to_string(result.maxValue);
        case RangeOperation::Minimum:
            return std::to_string(result.minValue);
    }

    return "";
}

// Calcula suma, resta, promedio, maximo y minimo para un rango rectangular.
OperationResult calculateRangeStats(
    const std::unordered_map<CellKey, int, CellKeyHash>& cells,
    CellCoord a,
    CellCoord b
) {
    const int rowStart = std::min(a.row, b.row);
    const int rowEnd = std::max(a.row, b.row);
    const int colStart = std::min(a.col, b.col);
    const int colEnd = std::max(a.col, b.col);

    OperationResult result;

    // Se recorre en orden de lectura: fila por fila, de izquierda a derecha.
    for (int row = rowStart; row <= rowEnd; ++row) {
        for (int col = colStart; col <= colEnd; ++col) {
            const auto it = cells.find(CellKey{row, col});
            if (it == cells.end()) {
                continue;
            }

            if (!result.hasValues) {
                result.sum = it->second;
                // La resta comienza con la primera celda ocupada y resta las siguientes.
                result.difference = it->second;
                result.minValue = it->second;
                result.maxValue = it->second;
                result.hasValues = true;
            } else {
                result.sum += it->second;
                result.difference -= it->second;
                result.minValue = std::min(result.minValue, it->second);
                result.maxValue = std::max(result.maxValue, it->second);
            }

            result.count++;
        }
    }

    return result;
}

// Elimina todas las celdas guardadas en una fila especifica.
int deleteRowCells(std::unordered_map<CellKey, int, CellKeyHash>& cells, int rowToDelete) {
    int removed = 0;

    for (auto it = cells.begin(); it != cells.end();) {
        if (it->first.row == rowToDelete) {
            it = cells.erase(it);
            removed++;
        } else {
            ++it;
        }
    }

    return removed;
}

// Elimina todas las celdas guardadas en una columna especifica.
int deleteColumnCells(std::unordered_map<CellKey, int, CellKeyHash>& cells, int colToDelete) {
    int removed = 0;

    for (auto it = cells.begin(); it != cells.end();) {
        if (it->first.col == colToDelete) {
            it = cells.erase(it);
            removed++;
        } else {
            ++it;
        }
    }

    return removed;
}

int main() {
    const int cols = 12;
    const int rows = 20;
    const int cellW = 70;
    const int cellH = 32;
    const int margin = 24;

    sf::RenderWindow window(
        sf::VideoMode({static_cast<unsigned int>(margin * 2 + cols * cellW), static_cast<unsigned int>(margin * 2 + rows * cellH)}),
        "MiniExcel"
    );

    std::unordered_map<CellKey, int, CellKeyHash> cells;
    int selectedRow = 0;
    int selectedCol = 0;
    std::string editBuffer;
    std::optional<CellCoord> rangeAnchor;
    std::string statusMessage;

    sf::Font font;
    bool fontLoaded = false;
#ifdef __APPLE__
    fontLoaded =
        font.openFromFile("/System/Library/Fonts/Supplemental/Arial.ttf") ||
        font.openFromFile("/System/Library/Fonts/Supplemental/Verdana.ttf");
#elif defined(_WIN32)
    fontLoaded =
        font.openFromFile("C:/Windows/Fonts/segoeui.ttf") ||
        font.openFromFile("C:/Windows/Fonts/arial.ttf");
#else
    fontLoaded =
        font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf") ||
        font.openFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");
#endif

    auto keyOfSelection = [&]() {
        return CellKey{selectedRow, selectedCol};
    };

    auto syncBufferFromCell = [&]() {
        const auto it = cells.find(keyOfSelection());
        if (it != cells.end()) {
            editBuffer = std::to_string(it->second);
        } else {
            editBuffer.clear();
        }
    };

    auto updateTitle = [&]() {
        const std::string selectedRange =
            rangeAnchor.has_value() ? " | rango " + rangeLabel(*rangeAnchor, CellCoord{selectedRow, selectedCol}) : "";
        const std::string status = statusMessage.empty() ? "" : " | " + statusMessage;
        const std::string title =
            "MiniExcel | celda " + cellLabel(selectedRow, selectedCol) +
            " | valor: " + (editBuffer.empty() ? std::string("(vacio)") : editBuffer) +
            selectedRange + status +
            " | Enter=guardar, Delete=celda, F=elim fila, C=elim col, Espacio=rango, S=suma, R=resta, P=prom, M=max, N=min";
        window.setTitle(title);
    };

    auto showOperation = [&](RangeOperation operation) {
        const CellCoord start = rangeAnchor.value_or(CellCoord{selectedRow, selectedCol});
        const CellCoord end{selectedRow, selectedCol};
        const OperationResult result = calculateRangeStats(cells, start, end);
        const std::string selectedRange = rangeLabel(start, end);
        const std::string name = operationName(operation);

        if (!result.hasValues) {
            statusMessage = name + " " + selectedRange + " = sin celdas";
        } else {
            statusMessage =
                name + " " + selectedRange + " = " + operationValue(operation, result) +
                " (" + std::to_string(result.count) + " celdas)";
        }

        // Mantiene visible el rango calculado despues de mostrar el resultado.
        rangeAnchor = start;
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
                    const CellKey k{selectedRow, selectedCol};
                    if (editBuffer.empty() || editBuffer == "-") {
                        cells.erase(k);
                    } else {
                        cells[k] = std::stoi(editBuffer);
                    }
                    statusMessage = "guardado " + cellLabel(selectedRow, selectedCol);
                } else if (key == sf::Keyboard::Key::Backspace) {
                    if (!editBuffer.empty()) {
                        editBuffer.pop_back();
                    }
                    statusMessage.clear();
                } else if (key == sf::Keyboard::Key::Delete) {
                    cells.erase(CellKey{selectedRow, selectedCol});
                    editBuffer.clear();
                    statusMessage = "borrado " + cellLabel(selectedRow, selectedCol);
                } else if (key == sf::Keyboard::Key::F) {
                    const int removed = deleteRowCells(cells, selectedRow);
                    editBuffer.clear();
                    rangeAnchor.reset();
                    statusMessage =
                        "fila " + std::to_string(selectedRow + 1) +
                        (removed == 0 ? " vacia" : " eliminada (" + std::to_string(removed) + " celdas)");
                } else if (key == sf::Keyboard::Key::C) {
                    const int removed = deleteColumnCells(cells, selectedCol);
                    editBuffer.clear();
                    rangeAnchor.reset();
                    statusMessage =
                        "columna " + columnLabel(selectedCol) +
                        (removed == 0 ? " vacia" : " eliminada (" + std::to_string(removed) + " celdas)");
                } else if (key == sf::Keyboard::Key::Space) {
                    rangeAnchor = CellCoord{selectedRow, selectedCol};
                    statusMessage = "inicio " + cellLabel(selectedRow, selectedCol);
                } else if (key == sf::Keyboard::Key::S) {
                    showOperation(RangeOperation::Sum);
                } else if (key == sf::Keyboard::Key::R) {
                    showOperation(RangeOperation::Subtract);
                } else if (key == sf::Keyboard::Key::P) {
                    showOperation(RangeOperation::Average);
                } else if (key == sf::Keyboard::Key::M) {
                    showOperation(RangeOperation::Maximum);
                } else if (key == sf::Keyboard::Key::N) {
                    showOperation(RangeOperation::Minimum);
                } else if (key == sf::Keyboard::Key::Escape) {
                    rangeAnchor.reset();
                    statusMessage.clear();
                }

                updateTitle();
            }

            if (const auto* textEntered = event->getIf<sf::Event::TextEntered>()) {
                const char32_t cp = textEntered->unicode;
                bool changed = false;

                if (cp >= U'0' && cp <= U'9') {
                    if (editBuffer.size() < 9) {
                        editBuffer.push_back(static_cast<char>(cp));
                        changed = true;
                    }
                } else if (cp == U'-') {
                    if (editBuffer.empty()) {
                        editBuffer.push_back('-');
                        changed = true;
                    }
                }

                if (changed) {
                    statusMessage.clear();
                    updateTitle();
                }
            }

            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePressed->button == sf::Mouse::Button::Left) {
                    const int mx = mousePressed->position.x;
                    const int my = mousePressed->position.y;
                    const int gx = mx - margin;
                    const int gy = my - margin;
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

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                sf::RectangleShape cell(sf::Vector2f(static_cast<float>(cellW - 1), static_cast<float>(cellH - 1)));
                cell.setPosition(sf::Vector2f(static_cast<float>(margin + c * cellW), static_cast<float>(margin + r * cellH)));

                const bool selected = (r == selectedRow && c == selectedCol);
                const bool inRange =
                    rangeAnchor.has_value() && isInsideRange(r, c, *rangeAnchor, CellCoord{selectedRow, selectedCol});
                const bool hasValue = cells.find(CellKey{r, c}) != cells.end();

                if (selected) {
                    cell.setFillColor(sf::Color(70, 120, 220));
                } else if (inRange) {
                    cell.setFillColor(sf::Color(58, 88, 105));
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
                    } else {
                        const auto it = cells.find(CellKey{r, c});
                        if (it != cells.end()) {
                            textValue = std::to_string(it->second);
                        }
                    }

                    if (!textValue.empty()) {
                        sf::Text text(font, textValue, 14);
                        text.setFillColor(sf::Color::White);
                        text.setPosition(sf::Vector2f(
                            static_cast<float>(margin + c * cellW + 6),
                            static_cast<float>(margin + r * cellH + 6)
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
