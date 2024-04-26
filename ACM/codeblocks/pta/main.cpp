#include <bits/stdc++.h>


using namespace std;

int main()
{
    int n;
    cin>>n;
    vector<string> st;
    map<string,bool> mp;
    for(int i=0; i<n; i++)
    {
        string str;
        cin>>str;
        mp[str]=true;
        st.push_back(str);
    }
    int tt,cnt =0;
    cin>>tt;
    string tar;
    vector<string> ss;
    while(cin>>tar)
    {

        ss.push_back(tar);
        if(tar[tar.size()-1]=='.')
            break;
    }


    string ans;
    for(int i=0; i<ss.size(); i++)
    {
        int f = 0;
        for(auto x:st)
        {
            if(ss[i]==x)
            {
                f =1;
                cnt++;
                ans+="<censored>";
            }
        }
        if(f==1)
            ans+=ss[i];
        if(i!=ss.size()-1)
        {
            ans+=" ";
        }

    }



    if(cnt<tt)
    {
        cout<<tar;
    }
    else
    {
        cout<<cnt<<endl;
        cout<<"He Xie Ni Quan Jia!";
    }

    return 0;
}
