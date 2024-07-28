#include <bits/stdc++.h>

using namespace std;

// Classe para representar uma materia
class Subject {
public:
    string name;
    int startTime;
    int endTime;

    Subject(string n, int start, int end) : name(n), startTime(start), endTime(end) {}

    // Operador < para comparacao
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
        sort(subjects.begin(), subjects.end());

        for (const auto& subj : subjects) {
            bool allocated = false;
            for (auto& room : roomAllocation) {
                if (room.second.back().endTime <= subj.startTime) {
                    room.second.push_back(subj);
                    allocated = true;
                    break;
                }
            }
            if (!allocated) {
                roomAllocation[roomAllocation.size()].push_back(subj);
            }
        }
    }

public:
    void addSubject() {
        string name;
        string startTimeStr, endTimeStr;
        int start, end;

        cout << "Nome da materia: ";
        cin >> name;
        cout << "Horario de inicio (hh:mm): ";
        cin >> startTimeStr;
        cout << "Horario de termino (hh:mm): ";
        cin >> endTimeStr;

        start = stoi(startTimeStr.substr(0, 2)) * 100 + stoi(startTimeStr.substr(3, 2));
        end = stoi(endTimeStr.substr(0, 2)) * 100 + stoi(endTimeStr.substr(3, 2));

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

// Funcoes para persistencia dos dados
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

// Funcao para gerar o relatorio
namespace Report {
    void generateReport(const Schedule& schedule) {
        const auto& roomAllocation = schedule.getRoomAllocation();

        for (const auto& room : roomAllocation) {
            cout << "Sala " << room.first + 1 << ":" << endl;
            for (const auto& subj : room.second) {
                cout << "  Materia: " << subj.name << " - Horario: " << subj.startTime << " as " << subj.endTime << endl;
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
        cout << "1. Adicionar materia" << endl;
        cout << "2. Gerar relatorio" << endl;
        cout << "3. Sair" << endl;
        cout << "Escolha uma opcao: ";
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
                cout << "Opcao invalida." << endl;
        }
    } while (opcao != 3);

    return 0;
}
