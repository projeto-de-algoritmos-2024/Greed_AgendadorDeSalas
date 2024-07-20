#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>

// Classe para representar uma matéria
class Subject {
public:
    std::string name;
    int startTime;
    int endTime;

    Subject(std::string n, int start, int end) : name(n), startTime(start), endTime(end) {}

    // Operador < para comparação
    bool operator<(const Subject& other) const {
        if (endTime != other.endTime)
            return endTime < other.endTime;
        return startTime < other.startTime;
    }
};

// Classe para gerenciar o agendamento
class Schedule {
private:
    std::vector<Subject> subjects;
    std::map<int, std::vector<Subject>> roomAllocation;

    void allocateRooms() {
        roomAllocation.clear();

        std::vector<std::pair<int, Subject>> events;
        for (const auto& subj : subjects) {
            events.emplace_back(subj.startTime, subj);
            events.emplace_back(subj.endTime, subj);
        }

        std::sort(events.begin(), events.end(), [](const auto& a, const auto& b) {
            return a.first < b.first;
        });

        int roomCount = 0;
        std::map<Subject, int> activeSubjects;

        for (const auto& event : events) {
            if (activeSubjects.find(event.second) == activeSubjects.end()) {
                int assignedRoom = roomCount++;
                roomAllocation[assignedRoom].push_back(event.second);
                activeSubjects[event.second] = assignedRoom;
            } else {
                int room = activeSubjects[event.second];
                if (event.first == event.second.endTime) {
                    activeSubjects.erase(event.second);
                }
            }
        }
    }

public:
    void addSubject() {
        std::string name;
        int start, end;

        std::cout << "Nome da matéria: ";
        std::cin >> name;
        std::cout << "Horário de início (em formato 24h, ex: 1300): ";
        std::cin >> start;
        std::cout << "Horário de término (em formato 24h, ex: 1500): ";
        std::cin >> end;

        subjects.emplace_back(name, start, end);
        allocateRooms();
    }

    const std::map<int, std::vector<Subject>>& getRoomAllocation() const {
        return roomAllocation;
    }

    std::vector<Subject>& getSubjects() {
        return subjects;
    }

    const std::vector<Subject>& getSubjects() const {
        return subjects;
    }
};

// Funções para persistência dos dados
namespace Persistence {
    bool loadSchedule(Schedule& schedule) {
        std::ifstream infile("schedule.dat", std::ios::binary);
        if (!infile) return false;

        int count;
        infile.read(reinterpret_cast<char*>(&count), sizeof(count));
        for (int i = 0; i < count; ++i) {
            int nameLen, startTime, endTime;
            infile.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            std::string name(nameLen, ' ');
            infile.read(&name[0], nameLen);
            infile.read(reinterpret_cast<char*>(&startTime), sizeof(startTime));
            infile.read(reinterpret_cast<char*>(&endTime), sizeof(endTime));
            schedule.getSubjects().emplace_back(name, startTime, endTime);
        }

        return true;
    }

    void saveSchedule(const Schedule& schedule) {
        std::ofstream outfile("schedule.dat", std::ios::binary);

        int count = schedule.getSubjects().size();
        outfile.write(reinterpret_cast<const char*>(&count), sizeof(count));
        for (const auto& subj : schedule.getSubjects()) {
            int nameLen = subj.name.size();
            outfile.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
            outfile.write(subj.name.c_str(), nameLen);
            outfile.write(reinterpret_cast<const char*>(&subj.startTime), sizeof(subj.startTime));
            outfile.write(reinterpret_cast<const char*>(&subj.endTime), sizeof(subj.endTime));
        }
    }
}

// Função para gerar o relatório
namespace Report {
    void generateReport(const Schedule& schedule) {
        const auto& roomAllocation = schedule.getRoomAllocation();

        for (const auto& room : roomAllocation) {
            std::cout << "Sala " << room.first + 1 << ":" << std::endl;
            for (const auto& subj : room.second) {
                std::cout << "  Matéria: " << subj.name << " - Horário: " << subj.startTime << " às " << subj.endTime << std::endl;
            }
        }
    }
}

int main() {
    Schedule schedule;

    // Carregar dados persistidos se existirem
    if (!Persistence::loadSchedule(schedule)) {
        std::cout << "Nenhum dado existente encontrado. Criando um novo agendamento." << std::endl;
    }

    int opcao;
    do {
        std::cout << "1. Adicionar matéria" << std::endl;
        std::cout << "2. Gerar relatório" << std::endl;
        std::cout << "3. Sair" << std::endl;
        std::cout << "Escolha uma opção: ";
        std::cin >> opcao;

        switch(opcao) {
            case 1:
                schedule.addSubject();
                Persistence::saveSchedule(schedule);
                break;
            case 2:
                Report::generateReport(schedule);
                break;
            case 3:
                std::cout << "Saindo..." << std::endl;
                break;
            default:
                std::cout << "Opção inválida." << std::endl;
        }
    } while (opcao != 3);

    return 0;
}
