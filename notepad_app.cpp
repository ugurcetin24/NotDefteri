#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>


class Note {
private:
    std::string title;
    std::string content;
    std::string timestamp;

public:
    // Yapıcı fonksiyon
    Note(const std::string& title, const std::string& content)
        : title(title), content(content) {
        // Zaman damgasını oluştur
        time_t now = time(0);
        char buffer[80];
        struct tm timeinfo;
        localtime_s(&timeinfo, &now);
        strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", &timeinfo);
        timestamp = buffer;
    }

    // Getter metodları
    std::string getTitle() const { return title; }
    std::string getContent() const { return content; }
    std::string getTimestamp() const { return timestamp; }

    // Notu formatlı göstermek için
    void display() const {
        std::cout << "\n===================================\n";
        std::cout << "Baslık: " << title << "\n";
        std::cout << "Olusturulma Zamanı: " << timestamp << "\n";
        std::cout << "-----------------------------------\n";
        std::cout << content << "\n";
        std::cout << "===================================\n";
    }
};

class Notepad {
private:
    std::vector<Note> notes;
    std::string filename;

public:
    // Yapıcı fonksiyon
    Notepad(const std::string& filename) : filename(filename) {
        loadNotes();
    }

    // Not ekle
    void addNote() {
        std::string title, content, line;

        std::cout << "Not basligini girin: ";
        std::cin.ignore();
        std::getline(std::cin, title);

        std::cout << "Not icerigini girin (bitirmek için bos satır girin):\n";
        while (std::getline(std::cin, line) && !line.empty()) {
            content += line + "\n";
        }

        Note newNote(title, content);
        notes.push_back(newNote);
        saveNotes();

        std::cout << "Not basariyla kaydedildi.\n";
    }

    // Tüm notları listele
    void listNotes() const {
        if (notes.empty()) {
            std::cout << "Henuz hic not eklenmemis.\n";
            return;
        }

        std::cout << "\nTum Notlar:\n";
        for (size_t i = 0; i < notes.size(); ++i) {
            std::cout << i + 1 << ". " << notes[i].getTitle()
                << " (" << notes[i].getTimestamp() << ")\n";
        }
    }

    // Belirli bir notu görüntüle
    void viewNote() const {
        listNotes();

        if (notes.empty()) {
            return;
        }

        int index;
        std::cout << "\nGoruntulemek istediginiz notun numarasini girin: ";
        std::cin >> index;

        if (index < 1 || index > static_cast<int>(notes.size())) {
            std::cout << "Gecersiz not numarasi!\n";
            return;
        }

        notes[index - 1].display();
    }

    // Not sil
    void deleteNote() {
        listNotes();

        if (notes.empty()) {
            return;
        }

        int index;
        std::cout << "\nSilmek istediginiz notun numarasini girin: ";
        std::cin >> index;

        if (index < 1 || index > static_cast<int>(notes.size())) {
            std::cout << "Gecersiz not numarasi!\n";
            return;
        }

        notes.erase(notes.begin() + index - 1);
        saveNotes();

        std::cout << "Not basarıyla silindi.\n";
    }

    // Arama yap
    void searchNotes() const {
        std::string keyword;
        std::cout << "Arama yapmak icin anahtar kelime girin: ";
        std::cin.ignore();
        std::getline(std::cin, keyword);

        std::vector<size_t> results;

        for (size_t i = 0; i < notes.size(); ++i) {
            if (notes[i].getTitle().find(keyword) != std::string::npos ||
                notes[i].getContent().find(keyword) != std::string::npos) {
                results.push_back(i);
            }
        }

        if (results.empty()) {
            std::cout << "Aramanizla eslesen not bulunamadi.\n";
            return;
        }

        std::cout << "\nArama Sonuclari:\n";
        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << i + 1 << ". " << notes[results[i]].getTitle() << "\n";
        }

        int index;
        std::cout << "\nGoruntulemek istediginiz sonucun numarasını girin (İptal icin 0): ";
        std::cin >> index;

        if (index >= 1 && index <= static_cast<int>(results.size())) {
            notes[results[index - 1]].display();
        }
    }

private:
    // Notları dosyadan yükle
    void loadNotes() {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return;
        }

        std::string line, title, content, timestamp;
        int state = 0;  // 0: başlık bekliyor, 1: zaman bekliyor, 2: içerik bekliyor

        while (std::getline(file, line)) {
            if (line == "---BEGIN_NOTE---") {
                state = 0;
                title = content = timestamp = "";
            }
            else if (line == "---END_NOTE---") {
                Note loadedNote(title, content);
                notes.push_back(loadedNote);
            }
            else if (state == 0) {
                title = line;
                state = 1;
            }
            else if (state == 1) {
                timestamp = line;
                state = 2;
            }
            else if (state == 2) {
                content += line + "\n";
            }
        }

        file.close();
    }

    // Notları dosyaya kaydet
    void saveNotes() const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Dosya açılamadı: " << filename << std::endl;
            return;
        }

        for (const auto& note : notes) {
            file << "---BEGIN_NOTE---\n";
            file << note.getTitle() << "\n";
            file << note.getTimestamp() << "\n";
            file << note.getContent();
            file << "---END_NOTE---\n";
        }

        file.close();
    }
};

void displayMenu() {
    std::cout << "\n===== Not Defteri Uygulamasi =====\n";
    std::cout << "1. Yeni Not Ekle\n";
    std::cout << "2. Notlari Listele\n";
    std::cout << "3. Not Goruntule\n";
    std::cout << "4. Not Sil\n";
    std::cout << "5. Notlarda Ara\n";
    std::cout << "0. Cikis\n";
    std::cout << "Seciminiz: ";
}

int main() {
    std::string filename = "notes.txt";
    Notepad notepad(filename);

    int choice;

    do {
        displayMenu();
        std::cin >> choice;

        switch (choice) {
        case 1:
            notepad.addNote();
            break;
        case 2:
            notepad.listNotes();
            break;
        case 3:
            notepad.viewNote();
            break;
        case 4:
            notepad.deleteNote();
            break;
        case 5:
            notepad.searchNotes();
            break;
        case 0:
            std::cout << "Uygulama kapatılıyor...\n";
            break;
        default:
            std::cout << "Gecersiz secim! Lütfen tekrar deneyin.\n";
        }
    } while (choice != 0);

    return 0;
}