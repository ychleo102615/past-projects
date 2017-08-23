#include <iostream>
#include <cmath>
#include <stack>
#include <ctime>
#include <cstdlib>

using namespace std;

struct loc
{
    int x;
    int y;
};

int atktest(loc a, loc b)
{
    // 1 means attack, 0 means safe
    if (a.x == b.x)
        return 1;
    else if(a.y == b.y)
        return 1;
    else if(abs(double(a.x-b.x)/(a.y-b.y)) == 1)
        return 1;
    else
        return 0;

};

int get_aknm_sum(loc qn[], int n)
{
    int sum = 0;

    for(int i=0;i<n-1;i++)//the last column does not need
    {
        for(int j=i+1;j<n;j++)
        {
            if(atktest(qn[i], qn[j]) == 1)
                sum++;
        }
    }
    return sum;
}

int main()
{
    int n = 1;
    cout << "Please enter  queen's number." << endl;
    cin >> n;
    int mode = 0;
    while(mode < 1 || mode > 3)
    {
        cout << "Please choose one mode from:\n\
                    1. DFS\n\
                    2. HC\n\
                    3. GA" << endl;
        cin >> mode;
        if(mode < 1 || mode > 3)
            cout << "Please enter one number between 3 and 1." << endl;
        else
            break;
    }


    //initialization
    loc queenloc[n];

    srand(time(NULL));

    clock_t start, stop;
    double suc_num = 0, iter = 30, atks_left = 0;
    int popu_num = 100, gi;

    int counter = 0;

    //for DFS initialization
    stack<loc> staloc;
    int ansnm = 0;

    switch(mode)
    {
    //DFS
    case 1:

        start = clock();
        for(int i=0;i<n;i++)
        {
            loc tmp;
            tmp.x = 0;
            tmp.y = i;
            staloc.push(tmp);
        }

        while(!staloc.empty())
        {
            if(n>30)
                if(++counter > 10000000)
            {
                counter = 0;
                cout << "still going...." << endl;
            }
            loc tmp = staloc.top();
            //if(tmp.x == 0)
            //    cout << "-------------------------------" << endl;
            //cout << "pop from stack" << endl;
            //cout << tmp.x << "," << tmp.y << endl;
            staloc.pop();
            queenloc[tmp.x] = tmp;//answer record, this will automatically cover the unneeded location

            //search in the same column
            for(int i=0;i<n;i++)
            {
                loc tmp2;//new testing location
                tmp2.x = tmp.x+1;
                tmp2.y = i;

                //compare every location recorded with new testing location
                int safe = 0;
                for(int j=0;j<=tmp.x;j++)
                {
                    if(atktest(queenloc[j], tmp2) == 1)
                    {
                        safe++;
                    }
                }
                if(safe == 0)
                {
                    if(tmp.x == n-1-1)//meets the second last column and find legal last column
                    {
                        cout << "****Answer founded.****" << endl;
                        ansnm++;
                        queenloc[n-1] = tmp2;
                        for(int k=0;k<n;k++)
                            cout << queenloc[k].x << "," << queenloc[k].y << endl;
                    }
                    else
                    {
                        staloc.push(tmp2);
                        /*
                        for(int k=0;k<=tmp.x;k++)
                            cout << queenloc[k].x << "," << queenloc[k].y << " ";
                        cout << " and new added: " << tmp2.x << "," << tmp2.y << endl;
                        */
                     }
                }
            }
        }
        stop = clock();
        cout << "Answer counts: " << ansnm << endl;
        cout << "Time: " << double(stop - start) / CLOCKS_PER_SEC << "sec." <<endl;
        break;

    //HC
    case 2:

        cout << "Please set iteration." << endl;
        cin >> iter;
        start = clock();

        for(int a=0;a<iter;a++)
        {
            cout << "------------ iteration " << a+1 << "----------" << endl;
            for(int i=0;i<n;i++)
            {
                queenloc[i].x = rand()%n;
                queenloc[i].y = i;
                //cout << queenloc[i].x << "," << queenloc[i].y << endl;
            }

            //find the state of the least number of attack
            while(true)
            {
                if(n>30)
                    if(++counter > 10000000)
                {
                    counter = 0;
                    cout << "still going...." << endl;
                }
                int max_change = 0, ori_akn = get_aknm_sum(queenloc, n);
                int temp;
                loc suc, tmp2; // 1 is in dfs
                //cout << "original attack number: " << ori_akn << endl;
                for(int i=0;i<n;i++)//for every column
                {
                    tmp2 = queenloc[i];
                    for(int j=0;j<n;j++)//for every row
                    {
                        queenloc[i].x = j;
                        temp = get_aknm_sum(queenloc,n);
                        if(ori_akn - temp > max_change)  //as n be bigger, cost of gks will be extremely big
                        {
                            //max_change = temp;
                            suc = queenloc[i];
                            //cout << "find bigger: " << suc.x << "," << suc.y << endl;
                            //cout << "cv: " << ori_akn - temp << "<---" << max_change << endl;
                            max_change = ori_akn - temp;
                         }

                        //should add situation find multiple max_change
                    }
                    queenloc[i] = tmp2;
                }

                // reach local max
                if(max_change == 0)
                    break;
                else
                {
                    //cout << "make substitute: " << suc.x << "," << suc.y << " -----> " << queenloc[suc.y].x << "," << queenloc[suc.y].y << endl;
                    queenloc[suc.y] = suc;
                }
            }

            int temp = get_aknm_sum(queenloc,n);
            if(temp == 0)
            {
                cout << "success." << endl;
                suc_num++;
            }
            else
            {
                cout << "fail, attacks left: " << temp << endl;
                atks_left += temp;
            }
        }

        stop = clock();
        cout << "\nSuccess rate: " << double(suc_num*100/iter) << "% " << "(" << suc_num << "/" << iter << ")." << endl;
        cout << "Average attacks: " << double(atks_left/iter) << " (" << atks_left << "/" << iter << ")." << endl;
        cout << "Average time: " << double(stop - start) / CLOCKS_PER_SEC/iter << "sec." << endl;

        break;

    //GA
    case 3:
        cout << "Please set iteration." << endl;
        cin >> iter;
        cout << "Please set population number." << endl;
        cin >> popu_num;
        cout << "Please set generation iteration." << endl;
        cin >> gi;
        start = clock();

//        loc population[popu_num][n];
//        int parent[popu_num/2];
//        loc child[popu_num/2][n];

        loc **population = new loc*[popu_num];
        for(int i=0;i<popu_num;i++)
            population[i] = new loc[n];
        int *parent = new int[popu_num/2];
        loc **child = new loc*[popu_num/2];
        for(int i=0;i<popu_num/2;i++)
            child[i] = new loc[n];

        int tmp, randpick, f;

        for(int a=0;a<iter;a++)
        {
            cout << "------------ iteration " << a+1 << "----------" << endl;
//cin >> tmp;
            //initialize pool
            for(int i=0;i<popu_num;i++)
            {
                for(int j=0;j<n;j++)
                {
                    population[i][j].x = rand()%n;
                    population[i][j].y = j;
                }
            }

            int index;
            bool sucf;
            for(int i=0;i<gi;i++)
            {
                if(n>30)
                    if(++counter > 10000000)
                {
                    counter = 0;
                    cout << "still going...." << endl;
                }
                //pick parents
                //tournament selection
                for(int j=0;j<popu_num/2;j++)
                {
                    tmp = 0;
                    for(int k=0;k<5;k++)
                    {
                        randpick = rand()%popu_num;
                        if(k==0)
                        {
                            tmp = get_aknm_sum(population[randpick], n);
                            parent[j] = randpick;
                        }
                        else
                            if(get_aknm_sum(population[randpick], n) < tmp)
                                parent[j] = randpick;
                    }
                }

                //crossover: set 1-point crossover
                //also with mutation
                for(int j=0;j<popu_num/4;j++)
                {
                    f = rand()%(n-1)+1;//choose one point randomly
                    for(int l=0;l<f;l++)
                    {
                        if(rand()%n != 0)
                            child[2*j][l]=population[parent[2*j]][l];
                        else
                            child[2*j][l].x = rand()%n;
                        if(rand()%n != 0)
                            child[2*j+1][l]=population[parent[2*j+1]][l];
                        else
                            child[2*j][l].x = rand()%n;
                    }
                    for(int l=f;l<n;l++)
                    {
                        if(rand()%n != 0)
                            child[2*j][l]=population[parent[2*j+1]][l];
                        else
                            child[2*j][l].x = rand()%n;
                        if(rand()%n != 0)
                            child[2*j+1][l]=population[parent[2*j]][l];
                        else
                            child[2*j][l].x = rand()%n;
                    }
                    /*for(int l=0;l<n;l++)
                    {
                        //cout << "parent:" << endl;
                        //cout << "x: " << population[parent[2*j+1]][l].x << " in " << 2*j+1 << endl;
                        //cout << "x: " << population[parent[2*j]][l].x << " in " << 2*j << endl;
                        //cout << "child:" << endl;
                        if(child[2*j][l].x >= n)
                        {
                            int temp;
                            cout << "x: " << population[parent[2*j+1]][l].x << " in " << 2*j+1 << endl;
                            cout << "x: " << population[parent[2*j]][l].x << " in " << 2*j << endl;
                            cout << "x: " << child[2*j][l].x << " in " << 2*j << endl;
                            cin >> temp;
                        }

                        if(child[2*j+1][l].x >= n)
                        {
                            int temp;
                            cout << "x: " << population[parent[2*j+1]][l].x << " in " << 2*j+1 << endl;
                            cout << "x: " << population[parent[2*j]][l].x << " in " << 2*j << endl;
                            cout << "x: " << child[2*j+1][l].x << " in " << 2*j+1 << endl;
                            cin >> temp;
                        }

                    }*/
                }

                //survivor , if is goal state --->  break
                int tmp3, tmp4, tmp5;
                sucf = false;
                for(int j=0;j<popu_num/2;j++)
                {
                    tmp3 = get_aknm_sum(population[2*j],n);
                    tmp4 = get_aknm_sum(population[2*j+1],n);
                    tmp5 = get_aknm_sum(child[j],n);
                    if(tmp3 <= tmp4 && tmp4 > tmp5)
                    {
                        for(int l=0;l<n;l++)
                        {
                            population[2*j+1][l] = child[j][l];
                        }
                    }
                    else if(tmp4 < tmp3 && tmp3 > tmp5)
                    {
                        for(int l=0;l<n;l++)
                        {
                            population[2*j][l] = child[j][l];
                        }
                    }
                    //else
                        //cout <<"other condition: " << " " <<tmp3<< " "<<tmp4<< " "<<tmp5<< endl;
                    if(tmp3==0)
                    {
                        sucf = true;
                        index = 2*j;
                        break;
                    }
                    else if(tmp4==0)
                    {
                        sucf = true;
                        index = 2*j+1;
                        break;
                    }
                    else if(tmp5==0)
                    {
                        sucf = true;
                        if(tmp3<=tmp4)
                            index = 2*j+1;
                        else
                            index = 2*j;

                        /*cout << "index: " << index << endl;
                        cout << "population in s3: " << get_aknm_sum(population[index],n) << endl;
                        cout << "j: " << j << endl;
                        cout << "child: " << get_aknm_sum(child[j],n) << endl;
                        */
                        break;
                    }

                }
                if(sucf)
                {
                    break;
                }
            }

            int bstpfm = n*n/2, tmp6;
            if(!sucf)
                for(int j=0;j<popu_num;j++)
                    if(tmp6 = get_aknm_sum(population[j],n) < bstpfm)
                    {
                        index = j;
                        bstpfm = tmp6;
                    }

            //calculation
            int temp = get_aknm_sum(population[index],n);
            if(temp == 0)
            {
                cout << "success." << endl;
                for(int l=0;l<n;l++)
                        {
                            cout << population[index][l].x << "," << population[index][l].y << endl;
                        }
                suc_num++;
            }
            else
            {
                cout << "fail, attacks left: " << temp << endl;
                atks_left += temp;
            }
        }

        stop = clock();
        cout << "\nSuccess rate: " << double(suc_num*100/iter) << "% " << "(" << suc_num << "/" << iter << ")." << endl;
        cout << "Average attacks: " << double(atks_left/iter) << " (" << atks_left << "/" << iter << ")." << endl;
        cout << "Average time: " << double(stop - start) / CLOCKS_PER_SEC/iter << "sec." << endl;

        for(int i = 0; i < n; i++)
        {
            delete []population[i];
            delete []child[i];
        }
        delete []population;
        delete []child;
        delete []parent;

        break;
    }

    return 0;
}
