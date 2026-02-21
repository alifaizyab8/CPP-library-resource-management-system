#pragma once
#include <string>

class Resource
{
private:
    int resourceId;
    std::string title;
    std::string author;
    std::string publisher;
    int publicationYear;
    std::string isbn;
    int categoryId;
    int resourceTypeId;
    int totalCopies;
    int availableCopies;
    std::string description;
    std::string addedDate;
    bool isActive;

public:
    Resource() : resourceId(0), publicationYear(0), categoryId(0), resourceTypeId(0),
                 totalCopies(0), availableCopies(0), isActive(false) {}

    Resource(int id, const std::string &title, const std::string &author, const std::string &pub,
             int year, const std::string &isbn, int catId, int typeId, int tCopies, int aCopies,
             const std::string &desc, const std::string &date, bool active)
        : resourceId(id), title(title), author(author), publisher(pub), publicationYear(year),
          isbn(isbn), categoryId(catId), resourceTypeId(typeId), totalCopies(tCopies),
          availableCopies(aCopies), description(desc), addedDate(date), isActive(active) {}

    // Getters
    int getResourceId() const { return resourceId; }
    std::string getTitle() const { return title; }
    std::string getAuthor() const { return author; }
    std::string getPublisher() const { return publisher; }
    int getPublicationYear() const { return publicationYear; }
    std::string getIsbn() const { return isbn; }
    int getCategoryId() const { return categoryId; }
    int getResourceTypeId() const { return resourceTypeId; }
    int getTotalCopies() const { return totalCopies; }
    int getAvailableCopies() const { return availableCopies; }
    std::string getDescription() const { return description; }
    std::string getAddedDate() const { return addedDate; }
    bool getIsActive() const { return isActive; }

    // Setters
    void setResourceId(int id) { resourceId = id; }
    void setTitle(const std::string &t) { title = t; }
    void setAuthor(const std::string &a) { author = a; }
    void setPublisher(const std::string &p) { publisher = p; }
    void setPublicationYear(int year) { publicationYear = year; }
    void setIsbn(const std::string &i) { isbn = i; }
    void setCategoryId(int id) { categoryId = id; }
    void setResourceTypeId(int id) { resourceTypeId = id; }
    void setTotalCopies(int copies) { totalCopies = copies; }
    void setAvailableCopies(int copies) { availableCopies = copies; }
    void setDescription(const std::string &d) { description = d; }
    void setAddedDate(const std::string &d) { addedDate = d; }
    void setIsActive(bool active) { isActive = active; }
};