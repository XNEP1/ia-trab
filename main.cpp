#include <bits/stdc++.h>
#include "minisat/core/Solver.h"
#include "minisat/core/SolverTypes.h"
using namespace std;

/*
As regras que definem a evolução de um estado para outro do jogo são:
  - toda célula morta com exatamente três vizinhos vivos torna-se viva;
  - toda célula viva com menos de dois vizinhos vivos morre;
  - toda célula viva com mais de três vizinhos vivos morre;
  - toda célula viva com dois ou três vizinhos vivos permanece viva.
*/

void vector_to_MinisatVec(vector<Minisat::Lit>& v, Minisat::vec<Minisat::Lit>& result){
    for(auto & lit : v){
        result.push(lit);
    }
}

void generateSubsets(const vector<Minisat::Lit>& A, int k, int index, 
                     vector<Minisat::Lit>& current, 
                     vector<vector<Minisat::Lit>>& result) {

    if (current.size() == k) {
        result.push_back(current);
        return;
    }

    for (int i = index; i < A.size(); ++i) {
        current.push_back(A[i]);
        generateSubsets(A, k, i + 1, current, result);
        current.pop_back();
    }
}

vector<vector<Minisat::Lit>> P(const vector<Minisat::Lit>& A, int k) {
    vector<vector<Minisat::Lit>> result;
    vector<Minisat::Lit> current;
    generateSubsets(A, k, 0, current, result);
    return result;
}

void addDeadCellsClauseWithRelaxation(Minisat::Solver& solver, vector<Minisat::Lit> const& cell_lits, vector<Minisat::Lit> const& relaxation_lits){
    for(int i=0; i<cell_lits.size(); ++i){
        Minisat::vec<Minisat::Lit> clause;
        clause.push(~(cell_lits[i]));
        clause.push(relaxation_lits[i]);
        solver.addClause(clause);
    }
}

void limitQntRelaxedClausules(Minisat::Solver& solver, vector<Minisat::Lit> const& relaxation_lits, int k){
    vector<vector<Minisat::Lit>> A = P(relaxation_lits, k);
    int q = 0;
    for (auto& subset : A) {
        Minisat::vec<Minisat::Lit> result;
        vector_to_MinisatVec(subset, result);

        Minisat::vec<Minisat::Lit> clause;
        for (size_t i = 0; i < result.size(); ++i) {
            clause.push(~result[i]);
        }
        solver.addClause(clause);
        q++;
    }
    printf("qnt=%d\n", q);
}

void loneliness(Minisat::Solver& solver, vector<Minisat::Lit> const& neighbors){
    vector<vector<Minisat::Lit>> A = P(neighbors, 7);
    for (auto& subset : A) {
        Minisat::vec<Minisat::Lit> result;
        vector_to_MinisatVec(subset, result);

        solver.addClause(result);
    }
}

void stagnation(Minisat::Solver& solver, vector<Minisat::Lit> const& neighbors, Minisat::Lit const& cell){
    vector<vector<Minisat::Lit>> A = P(neighbors, 2);
    for (auto& subset : A) {
        Minisat::vec<Minisat::Lit> result;
        vector_to_MinisatVec(subset, result);

        Minisat::vec<Minisat::Lit> clause;
        clause.push(cell);

        for (size_t k = 0; k < result.size(); ++k) {
            clause.push(~result[k]);
        }

        Minisat::vec<Minisat::Lit> temp;
        std::unordered_set<int> c_values;
        for (int i = 0; i < result.size(); ++i) {
            c_values.insert(result[i].x);
        }
        for (int i = 0; i < neighbors.size(); ++i) {
            if (c_values.find(neighbors[i].x) == c_values.end()) { // Se não está em c
                temp.push(neighbors[i]);
            }
        }
        for (size_t k = 0; k < temp.size(); ++k) {
            clause.push(temp[k]);
        }

        solver.addClause(clause);
    }
}

void overcrowding(Minisat::Solver& solver, vector<Minisat::Lit> const& neighbors){
    vector<vector<Minisat::Lit>> A = P(neighbors, 4);
    for (auto& subset : A) {
        Minisat::vec<Minisat::Lit> result;
        vector_to_MinisatVec(subset, result);

        Minisat::vec<Minisat::Lit> clause;
        for (size_t k = 0; k < result.size(); ++k) {
            clause.push(~result[k]);
        }
        solver.addClause(clause);
    }
}

void preservation(Minisat::Solver& solver, vector<Minisat::Lit> const& neighbors, Minisat::Lit const& cell){
    vector<vector<Minisat::Lit>> A = P(neighbors, 2);
    for (auto& subset : A) {
        Minisat::vec<Minisat::Lit> result;
        vector_to_MinisatVec(subset, result);

        Minisat::vec<Minisat::Lit> clause;
        clause.push(~cell);

        for (size_t k = 0; k < result.size(); ++k) {
            clause.push(~result[k]);
        }

        Minisat::vec<Minisat::Lit> temp;
        std::unordered_set<int> c_values;
        for (int i = 0; i < result.size(); ++i) {
            c_values.insert(result[i].x);
        }
        for (int i = 0; i < neighbors.size(); ++i) {
            if (c_values.find(neighbors[i].x) == c_values.end()) { // Se não está em c
                temp.push(neighbors[i]);
            }
        }
        for (size_t k = 0; k < temp.size(); ++k) {
            clause.push(temp[k]);
        }

        solver.addClause(clause);
    }
}

void life(Minisat::Solver& solver, vector<Minisat::Lit> const& neighbors){
    vector<vector<Minisat::Lit>> A = P(neighbors, 3);
    for (auto& subset : A) {
        Minisat::vec<Minisat::Lit> result;
        vector_to_MinisatVec(subset, result);

        Minisat::vec<Minisat::Lit> clause;

        for (size_t k = 0; k < result.size(); ++k) {
            clause.push(~result[k]);
        }

        Minisat::vec<Minisat::Lit> temp;
        std::unordered_set<int> c_values;
        for (int i = 0; i < result.size(); ++i) {
            c_values.insert(result[i].x);
        }
        for (int i = 0; i < neighbors.size(); ++i) {
            if (c_values.find(neighbors[i].x) == c_values.end()) { // Se não está em c
                temp.push(neighbors[i]);
            }
        }
        for (size_t k = 0; k < temp.size(); ++k) {
            clause.push(temp[k]);
        }

        solver.addClause(clause);
    }
}

int main(void){

    Minisat::Solver solver;

    int n,m;
    cin >> n >> m;
    n+=2; m+=2; // adiciona as bordas

    vector<vector<int>> board(n, vector<int>(m, 0));
    for(int i=1; i<n-1; ++i){
        for(int j=1; j<m-1; ++j){
            cin >> board[i][j];
        }
    }

    // criarei uma clausula para cada celula que obriga ela a estar morta caso a variavel de relaxamento seja falso.
    // a ideia é ir aumentando a quantidade de variaveis relaxadas até que haja um resultado SAT.
    vector<Minisat::Var> relaxation_vars; 
    vector<Minisat::Lit> cells_lit;


    vector<vector<Minisat::Var>> previous(n, vector<Minisat::Var>(m));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            previous[i][j] = solver.newVar();
            relaxation_vars.push_back(solver.newVar());
            cells_lit.push_back(Minisat::mkLit(previous[i][j]));
        }
    }

    vector<Minisat::Lit> relaxation_lits_1;
    for(auto& var : relaxation_vars){
        relaxation_lits_1.push_back(Minisat::mkLit(var));
    }
    addDeadCellsClauseWithRelaxation(solver, cells_lit, relaxation_lits_1);

    printf("Ponto 1\n");

    vector<Minisat::Lit> relaxation_lits_2;
    for(auto& var : relaxation_vars){
        relaxation_lits_2.push_back(Minisat::mkLit(var));
    }
    limitQntRelaxedClausules(solver, relaxation_lits_2, 5);

    printf("Ponto 2\n");

    // bordas são todas mortas
    for (size_t i = 0; i < n; ++i) {
        solver.addClause(~Minisat::mkLit(previous[i][0]));
        solver.addClause(~Minisat::mkLit(previous[i][m-1]));
    }
    for (size_t j = 0; j < m; ++j) {
        solver.addClause(~Minisat::mkLit(previous[0][j]));
        solver.addClause(~Minisat::mkLit(previous[n-1][j]));
    }

    for (int i = 1; i < n - 1; ++i) { // Ignorar bordas
        for (int j = 1; j < m - 1; ++j) {

            Minisat::Lit cell = Minisat::mkLit(previous[i][j]);

            // Soma dos vizinhos no passado
            vector<Minisat::Lit> neighbors;
            for (int di = -1; di <= 1; ++di) {
                for (int dj = -1; dj <= 1; ++dj) {
                    if (di != 0 || dj != 0) {
                        neighbors.push_back(Minisat::mkLit(previous[i + di][j + dj]));
                    }
                }
            }

            if(board[i][j] == 1){ // vivo 
                loneliness(solver, neighbors);
                stagnation(solver, neighbors, cell);
                overcrowding(solver, neighbors);
            } else { // morto 
                preservation(solver, neighbors, cell);
                life(solver, neighbors);
            }
        }
    }

    //printf("Resolvendo...\n");

    if (solver.solve()) {
        // Solução encontrada
        cout << n -2 << " " << m -2 << endl;
        for (int i = 1; i < n - 1; ++i) { // Ignorar bordas
            for (int j = 1; j < m - 1; ++j) {
                cout << (solver.modelValue(previous[i][j]) == (Minisat::lbool((uint8_t)0)) ? 1 : 0) << " ";
            }
            cout << endl;
        }
    } else {
        cout << "Sem solução" << endl;
    }

    return 0;
}