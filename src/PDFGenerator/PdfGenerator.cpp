#include "PDFGenerator.h"
#include <fstream>
#include <cstdlib>
#include <iostream>

void makePdf(std::string filename, std::string title, std::string contentString)
{
    // 1. Create the blueprint
    std::ofstream html("temp_file.html");

    html << "<html><head><style>"
         << "body { font-family: 'Courier New', Courier, monospace; margin: 50px; }"
         << "h2 { color: #2c3e50; border-bottom: 1px solid #ccc; }"
         << ".content { white-space: pre-wrap; font-size: 14px; line-height: 1.5; }"
         << "</style></head><body>"
         << "<h2>" << title << "</h2>"
         << "<div class='content'>" << contentString << "</div>"
         << "</body></html>";

    html.close();

    // 2. Convert using the exe in your build folder
    std::string command = "wkhtmltopdf --quiet temp_file.html " + filename;
    std::system(command.c_str());

    // 3. Cleanup
    std::remove("temp_file.html");
}