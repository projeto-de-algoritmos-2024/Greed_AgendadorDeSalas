#include <bits/stdc++.h>

using namespace std;

// Classe para representar uma matéria
class Subject {
public:
    string name;
    int startTime;
    int endTime;

    Subject(string n, int start, int end) : name(n), startTime(start), endTime(end) {}

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
    vector<Subject> subjects;
    map<int, vector<Subject>> roomAllocation;

    void allocateRooms() {
        roomAllocation.clear();

        vector<pair<int, Subject>> events;
        for (const auto& subj : subjects) {
            events.emplace_back(subj.startTime, subj);
            events.emplace_back(subj.endTime, subj);
        }

        sort(events.begin(), events.end(), [](const auto& a, const auto& b) {
            return a.first < b.first;
        });

        int roomCount = 0;
        map<Subject, int> activeSubjects;

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
        string name;
        int start, end;

        cout << "Nome da matéria: ";
        cin >> name;
        cout << "Horário de início (em formato 24h, ex: 1300): ";
        cin >> start;
        cout << "Horário de término (em formato 24h, ex: 1500): ";
        cin >> end;

        subjects.emplace_back(name, start, end);
        allocateRooms();
    }

    const map<int, vector<Subject>>& getRoomAllocation() const {
        return roomAllocation;
    }

    vector<Subject>& getSubjects() {
        return subjects;
    }

    const vector<Subject>& getSubjects() const {
        return subjects;
    }
};

// Funções para persistência dos dados
namespace Persistence {
    bool loadSchedule(Schedule& schedule) {
        ifstream infile("schedule.dat", ios::binary);
        if (!infile) return false;

        int count;
        infile.read(reinterpret_cast<char*>(&count), sizeof(count));
        for (int i = 0; i < count; ++i) {
            int nameLen, startTime, endTime;
            infile.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            string name(nameLen, ' ');
            infile.read(&name[0], nameLen);
            infile.read(reinterpret_cast<char*>(&startTime), sizeof(startTime));
            infile.read(reinterpret_cast<char*>(&endTime), sizeof(endTime));
            schedule.getSubjects().emplace_back(name, startTime, endTime);
        }

        return true;
    }

    void saveSchedule(const Schedule& schedule) {
        ofstream outfile("schedule.dat", ios::binary);

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
            cout << "Sala " << room.first + 1 << ":" << endl;
            for (const auto& subj : room.second) {
                cout << "  Matéria: " << subj.name << " - Horário: " << subj.startTime << " às " << subj.endTime << endl;
            }
        }
    }
}

int main() {
    Schedule schedule;

    // Carregar dados persistidos se existirem
    if (!Persistence::loadSchedule(schedule)) {
        cout << "Nenhum dado existente encontrado. Criando um novo agendamento." << endl;
    }

    int opcao;
    do {
        cout << "1. Adicionar matéria" << endl;
        cout << "2. Gerar relatório" << endl;
        cout << "3. Sair" << endl;
        cout << "Escolha uma opção: ";
        cin >> opcao;

        switch(opcao) {
            case 1:
                schedule.addSubject();
                Persistence::saveSchedule(schedule);
                break;
            case 2:
                Report::generateReport(schedule);
                break;
            case 3:
                cout << "Saindo..." << endl;
                break;
            default:
                cout << "Opção inválida." << endl;
        }
    } while (opcao != 3);

    return 0;
}
