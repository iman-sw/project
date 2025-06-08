#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <iterator>

using namespace std;

// Forward declarations
class Artist;
class Playlist;
class User;
class Admin;
class Song;

// Enum for playback modes
enum class PlaybackMode {
    SEQUENTIAL,
    RANDOM,
    REPEAT
};

// Global collections
vector<Song*> allSongs;
vector<Playlist*> allPlaylists;
vector<Artist*> allArtists;
vector<User*> allUsers;
Admin* admin = nullptr;

// Utility functions
void trim(string& str) {
    str.erase(str.begin(), find_if(str.begin(), str.end(), [](int ch) { return !isspace(ch); }));
    str.erase(find_if(str.rbegin(), str.rend(), [](int ch) { return !isspace(ch); }).base(), str.end());
}

vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        trim(token);
        if (!token.empty()) tokens.push_back(token);
    }
    return tokens;
}

// Song class definition
class Song {
private:
    string title;
    Artist* artist;
    int releaseYear;
    string genre;

public:
    Song(const string& title, Artist* artist, int year, const string& genre)
        : title(title), artist(artist), releaseYear(year), genre(genre) {}

    string getTitle() const { return title; }
    Artist* getArtist() const { return artist; }
    int getReleaseYear() const { return releaseYear; }
    string getGenre() const { return genre; }

    void display() const;
};

// Artist class definition
class Artist {
private:
    string name;
    vector<Song*> songs;
    vector<Playlist*> albums;

public:
    Artist(const string& name) : name(name) {}

    string getName() const { return name; }
    int getAlbumCount() const { return albums.size(); }
    int getSongCount() const { return songs.size(); }
    const vector<Song*>& getSongs() const { return songs; }
    const vector<Playlist*>& getAlbums() const { return albums; }

    void addSong(Song* song) {
        if (find(songs.begin(), songs.end(), song) == songs.end()) {
            songs.push_back(song);
        }
    }

    void addAlbum(Playlist* album) {
        if (find(albums.begin(), albums.end(), album) == albums.end()) {
            albums.push_back(album);
        }
    }

    void display() const;
};

// Playlist class definition
class Playlist {
private:
    string name;
    vector<Song*> songs;
    User* creator;
    bool isPublic;

public:
    Playlist(const string& name, User* creator, bool isPublic = true)
        : name(name), creator(creator), isPublic(isPublic) {}

    string getName() const { return name; }
    int getSongCount() const { return songs.size(); }
    const vector<Song*>& getSongs() const { return songs; }
    User* getCreator() const { return creator; }
    bool getIsPublic() const { return isPublic; }

    void addSong(Song* song) {
        if (find(songs.begin(), songs.end(), song) == songs.end()) {
            songs.push_back(song);
        }
    }

    void removeSong(Song* song) {
        songs.erase(remove(songs.begin(), songs.end(), song), songs.end());
    }

    void display() const;
};

// User base class definition
class User {
protected:
    string username;
    string password;
    vector<Song*> favoriteSongs;
    vector<Playlist*> favoritePlaylists;
    vector<Playlist*> personalPlaylists;
    Playlist* currentPlaylist = nullptr;
    Song* currentSong = nullptr;
    PlaybackMode playbackMode = PlaybackMode::SEQUENTIAL;
    bool isLooping = false;

public:
    User(const string& username, const string& password)
        : username(username), password(password) {}

    string getUsername() const { return username; }
    string getPassword() const { return password; }
    const vector<Song*>& getFavoriteSongs() const { return favoriteSongs; }
    const vector<Playlist*>& getFavoritePlaylists() const { return favoritePlaylists; }
    const vector<Playlist*>& getPersonalPlaylists() const { return personalPlaylists; }
    Playlist* getCurrentPlaylist() const { return currentPlaylist; }
    Song* getCurrentSong() const { return currentSong; }
    bool isLoopingEnabled() const { return isLooping; }

    void setCurrentSong(Song* song) { currentSong = song; }

    bool authenticate(const string& uname, const string& pwd) const {
        return username == uname && password == pwd;
    }

    void createPlaylist(const string& name, bool isPublic = true);
    void deletePlaylist(Playlist* playlist);

    void addFavoriteSong(Song* song) {
        if (find(favoriteSongs.begin(), favoriteSongs.end(), song) == favoriteSongs.end()) {
            favoriteSongs.push_back(song);
        }
    }

    void removeFavoriteSong(Song* song) {
        favoriteSongs.erase(remove(favoriteSongs.begin(), favoriteSongs.end(), song), favoriteSongs.end());
    }

    void addFavoritePlaylist(Playlist* playlist) {
        if (find(favoritePlaylists.begin(), favoritePlaylists.end(), playlist) == favoritePlaylists.end()) {
            favoritePlaylists.push_back(playlist);
        }
    }

    void removeFavoritePlaylist(Playlist* playlist) {
        favoritePlaylists.erase(remove(favoritePlaylists.begin(), favoritePlaylists.end(), playlist), favoritePlaylists.end());
    }

    void setCurrentPlaylist(Playlist* playlist) {
        currentPlaylist = playlist;
        if (playlist && !playlist->getSongs().empty()) {
            currentSong = playlist->getSongs()[0];
        }
        else {
            currentSong = nullptr;
        }
    }

    void setPlaybackMode(PlaybackMode mode) {
        playbackMode = mode;
    }

    void toggleLoop() {
        isLooping = !isLooping;
    }

    Song* getNextSong();
    Song* getPreviousSong();

    vector<Song*> searchSongs(const string& query) const;
    vector<Playlist*> searchPlaylists(const string& query) const;

    void displayFavoriteSongs() const;
    void displayFavoritePlaylists() const;
    void displayPersonalPlaylists() const;

    virtual void displayMenu();
    virtual ~User();
};

// Admin class definition
class Admin : public User {
public:
    Admin(const string& username, const string& password)
        : User(username, password) {}

    void addSong(const string& title, Artist* artist, int year, const string& genre);
    void removeSong(Song* song);
    void createArtist(const string& name);
    void removeArtist(Artist* artist);
    void createAlbum(Artist* artist, const string& name);

    void displayMenu() override;
};

// Implementations of methods that require complete types
void Song::display() const {
    cout << "Title: " << title << endl;
    cout << "Artist: " << artist->getName() << endl;
    cout << "Year: " << releaseYear << endl;
    cout << "Genre: " << genre << endl;
}

void Artist::display() const {
    cout << "Artist: " << name << endl;
    cout << "Total Songs: " << songs.size() << endl;
    cout << "Total Albums: " << albums.size() << endl;

    cout << "\nPopular Songs:" << endl;
    for (int i = 0; i < min(5, (int)songs.size()); i++) {
        cout << "- " << songs[i]->getTitle() << endl;
    }

    cout << "\nAlbums:" << endl;
    for (const auto& album : albums) {
        cout << "- " << album->getName() << endl;
    }
}

void Playlist::display() const {
    cout << "Playlist: " << name << endl;
    cout << "Creator: " << creator->getUsername() << endl;
    cout << "Songs (" << songs.size() << "):" << endl;
    for (const auto& song : songs) {
        cout << "- " << song->getTitle() << " by " << song->getArtist()->getName() << endl;
    }
}

void User::createPlaylist(const string& name, bool isPublic) {
    personalPlaylists.push_back(new Playlist(name, this, isPublic));
}

void User::deletePlaylist(Playlist* playlist) {
    auto it = find(personalPlaylists.begin(), personalPlaylists.end(), playlist);
    if (it != personalPlaylists.end()) {
        delete* it;
        personalPlaylists.erase(it);
    }
}

Song* User::getNextSong() {
    if (!currentPlaylist || currentPlaylist->getSongs().empty()) return nullptr;

    const auto& songs = currentPlaylist->getSongs();
    auto it = find(songs.begin(), songs.end(), currentSong);

    if (it == songs.end()) return nullptr;

    switch (playbackMode) {
    case PlaybackMode::SEQUENTIAL: {
        if (next(it) != songs.end()) {
            return *(next(it));
        }
        else if (isLooping) {
            return songs[0];
        }
        break;
    }
    case PlaybackMode::RANDOM: {
        return songs[rand() % songs.size()];
    }
    case PlaybackMode::REPEAT: {
        return currentSong;
    }
    }
    return nullptr;
}

Song* User::getPreviousSong() {
    if (!currentPlaylist || currentPlaylist->getSongs().empty()) return nullptr;

    const auto& songs = currentPlaylist->getSongs();
    auto it = find(songs.begin(), songs.end(), currentSong);

    if (it == songs.end()) return nullptr;

    if (playbackMode == PlaybackMode::SEQUENTIAL) {
        if (it != songs.begin()) {
            return *(prev(it));
        }
        else if (isLooping) {
            return songs.back();
        }
    }
    return currentSong;
}

vector<Song*> User::searchSongs(const string& query) const {
    vector<Song*> results;
    for (const auto& song : allSongs) {
        if (song->getTitle().find(query) != string::npos ||
            song->getArtist()->getName().find(query) != string::npos) {
            results.push_back(song);
        }
    }
    return results;
}

vector<Playlist*> User::searchPlaylists(const string& query) const {
    vector<Playlist*> results;
    for (const auto& playlist : allPlaylists) {
        if (playlist->getName().find(query) != string::npos) {
            results.push_back(playlist);
        }
    }
    return results;
}

void User::displayFavoriteSongs() const {
    cout << "Favorite Songs:" << endl;
    for (const auto& song : favoriteSongs) {
        cout << "- " << song->getTitle() << " by " << song->getArtist()->getName() << endl;
    }
}

void User::displayFavoritePlaylists() const {
    cout << "Favorite Playlists:" << endl;
    for (const auto& playlist : favoritePlaylists) {
        cout << "- " << playlist->getName() << " by " << playlist->getCreator()->getUsername() << endl;
    }
}

void User::displayPersonalPlaylists() const {
    cout << "Personal Playlists:" << endl;
    for (const auto& playlist : personalPlaylists) {
        cout << "- " << playlist->getName() << " (" << playlist->getSongCount() << " songs)" << endl;
    }
}

void User::displayMenu() {
    cout << "\nWelcome, " << username << "!" << endl;
    cout << "1. Browse Songs" << endl;
    cout << "2. Browse Playlists" << endl;
    cout << "3. My Favorite Songs" << endl;
    cout << "4. My Favorite Playlists" << endl;
    cout << "5. My Playlists" << endl;
    cout << "6. Search" << endl;
    cout << "7. Play Music" << endl;
    cout << "8. Logout" << endl;
}

User::~User() {
    for (auto& playlist : personalPlaylists) {
        delete playlist;
    }
}

void Admin::addSong(const string& title, Artist* artist, int year, const string& genre) {
    Song* song = new Song(title, artist, year, genre);
    allSongs.push_back(song);
    artist->addSong(song);
}

void Admin::removeSong(Song* song) {
    // Remove from global list
    allSongs.erase(remove(allSongs.begin(), allSongs.end(), song), allSongs.end());

    // Remove from artist's songs
    Artist* artist = song->getArtist();
    vector<Song*>& artistSongs = const_cast<vector<Song*>&>(artist->getSongs());
    artistSongs.erase(remove(artistSongs.begin(), artistSongs.end(), song), artistSongs.end());

    // Remove from all playlists
    for (auto& playlist : allPlaylists) {
        playlist->removeSong(song);
    }

    // Remove from users' favorites
    for (auto& user : allUsers) {
        user->removeFavoriteSong(song);
    }

    delete song;
}

void Admin::createArtist(const string& name) {
    allArtists.push_back(new Artist(name));
}

void Admin::removeArtist(Artist* artist) {
    // First remove all songs by this artist
    vector<Song*> artistSongs = artist->getSongs();
    for (auto& song : artistSongs) {
        removeSong(song);
    }

    // Then remove the artist
    allArtists.erase(remove(allArtists.begin(), allArtists.end(), artist), allArtists.end());
    delete artist;
}

void Admin::createAlbum(Artist* artist, const string& name) {
    Playlist* album = new Playlist(name, this, true);
    allPlaylists.push_back(album);
    artist->addAlbum(album);
}

void Admin::displayMenu() {
    cout << "\nAdmin Panel - Welcome, " << username << "!" << endl;
    cout << "1. Add Song" << endl;
    cout << "2. Create Artist" << endl;
    cout << "3. Create Album" << endl;
    cout << "4. Browse Songs" << endl;
    cout << "5. Browse Playlists" << endl;
    cout << "6. Browse Artists" << endl;
    cout << "7. Logout" << endl;
}

// UI functions
void displaySongs(const vector<Song*>& songs) {
    cout << "\nSongs (" << songs.size() << "):" << endl;
    for (size_t i = 0; i < songs.size(); i++) {
        cout << i + 1 << ". " << songs[i]->getTitle() << " by " << songs[i]->getArtist()->getName() << endl;
    }
}

void displayPlaylists(const vector<Playlist*>& playlists) {
    cout << "\nPlaylists (" << playlists.size() << "):" << endl;
    for (size_t i = 0; i < playlists.size(); i++) {
        cout << i + 1 << ". " << playlists[i]->getName() << " by " << playlists[i]->getCreator()->getUsername()
            << " (" << playlists[i]->getSongCount() << " songs)" << endl;
    }
}

void displayArtists(const vector<Artist*>& artists) {
    cout << "\nArtists (" << artists.size() << "):" << endl;
    for (size_t i = 0; i < artists.size(); i++) {
        cout << i + 1 << ". " << artists[i]->getName() << " (" << artists[i]->getSongCount() << " songs)" << endl;
    }
}

void userMenu(User* user) {
    while (true) {
        user->displayMenu();
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1: { // Browse Songs
            displaySongs(allSongs);

            cout << "\nOptions:" << endl;
            cout << "1. Filter by artist" << endl;
            cout << "2. Filter by genre" << endl;
            cout << "3. Filter by year" << endl;
            cout << "4. Sort A-Z" << endl;
            cout << "5. Sort by year" << endl;
            cout << "6. Back" << endl;

            int filterChoice;
            cin >> filterChoice;
            cin.ignore();

            vector<Song*> filteredSongs = allSongs;

            if (filterChoice == 1) {
                cout << "Enter artist name: ";
                string artistName;
                getline(cin, artistName);

                filteredSongs.erase(remove_if(filteredSongs.begin(), filteredSongs.end(),
                    [&artistName](Song* song) {
                        return song->getArtist()->getName().find(artistName) == string::npos;
                    }), filteredSongs.end());
            }
            else if (filterChoice == 2) {
                cout << "Enter genre: ";
                string genre;
                getline(cin, genre);

                filteredSongs.erase(remove_if(filteredSongs.begin(), filteredSongs.end(),
                    [&genre](Song* song) {
                        return song->getGenre().find(genre) == string::npos;
                    }), filteredSongs.end());
            }
            else if (filterChoice == 3) {
                cout << "Enter year: ";
                int year;
                cin >> year;
                cin.ignore();

                filteredSongs.erase(remove_if(filteredSongs.begin(), filteredSongs.end(),
                    [year](Song* song) {
                        return song->getReleaseYear() != year;
                    }), filteredSongs.end());
            }
            else if (filterChoice == 4) {
                sort(filteredSongs.begin(), filteredSongs.end(),
                    [](Song* a, Song* b) { return a->getTitle() < b->getTitle(); });
            }
            else if (filterChoice == 5) {
                sort(filteredSongs.begin(), filteredSongs.end(),
                    [](Song* a, Song* b) { return a->getReleaseYear() < b->getReleaseYear(); });
            }

            if (filterChoice != 6) {
                displaySongs(filteredSongs);

                cout << "\nSelect a song to add to favorites (0 to cancel): ";
                int songChoice;
                cin >> songChoice;
                cin.ignore();

                if (songChoice > 0 && songChoice <= static_cast<int>(filteredSongs.size())) {
                    user->addFavoriteSong(filteredSongs[songChoice - 1]);
                    cout << "Song added to favorites!" << endl;
                }
            }
            break;
        }
        case 2: { // Browse Playlists
            displayPlaylists(allPlaylists);

            cout << "\nSelect a playlist to view (0 to cancel): ";
            int plChoice;
            cin >> plChoice;
            cin.ignore();

            if (plChoice > 0 && plChoice <= static_cast<int>(allPlaylists.size())) {
                Playlist* selected = allPlaylists[plChoice - 1];
                selected->display();

                cout << "\n1. Add to favorites" << endl;
                cout << "2. Play this playlist" << endl;
                cout << "3. Back" << endl;

                int plAction;
                cin >> plAction;
                cin.ignore();

                if (plAction == 1) {
                    user->addFavoritePlaylist(selected);
                    cout << "Playlist added to favorites!" << endl;
                }
                else if (plAction == 2) {
                    user->setCurrentPlaylist(selected);
                    cout << "Playlist set as current!" << endl;
                }
            }
            break;
        }
        case 3: // Favorite Songs
            user->displayFavoriteSongs();
            break;
        case 4: // Favorite Playlists
            user->displayFavoritePlaylists();
            break;
        case 5: { // Personal Playlists
            user->displayPersonalPlaylists();

            cout << "\n1. Create new playlist" << endl;
            cout << "2. Manage existing playlist" << endl;
            cout << "3. Back" << endl;

            int plChoice;
            cin >> plChoice;
            cin.ignore();

            if (plChoice == 1) {
                cout << "Enter playlist name: ";
                string name;
                getline(cin, name);
                user->createPlaylist(name);
                cout << "Playlist created!" << endl;
            }
            else if (plChoice == 2 && !user->getPersonalPlaylists().empty()) {
                cout << "Select playlist to manage: ";
                int plNum;
                cin >> plNum;
                cin.ignore();

                if (plNum > 0 && plNum <= static_cast<int>(user->getPersonalPlaylists().size())) {
                    Playlist* pl = user->getPersonalPlaylists()[plNum - 1];
                    pl->display();

                    cout << "\n1. Add song" << endl;
                    cout << "2. Remove song" << endl;
                    cout << "3. Delete playlist" << endl;
                    cout << "4. Back" << endl;

                    int manageChoice;
                    cin >> manageChoice;
                    cin.ignore();

                    if (manageChoice == 1) {
                        displaySongs(allSongs);
                        cout << "Select song to add: ";
                        int songNum;
                        cin >> songNum;
                        cin.ignore();

                        if (songNum > 0 && songNum <= static_cast<int>(allSongs.size())) {
                            pl->addSong(allSongs[songNum - 1]);
                            cout << "Song added to playlist!" << endl;
                        }
                    }
                    else if (manageChoice == 2 && !pl->getSongs().empty()) {
                        displaySongs(pl->getSongs());
                        cout << "Select song to remove: ";
                        int songNum;
                        cin >> songNum;
                        cin.ignore();

                        if (songNum > 0 && songNum <= static_cast<int>(pl->getSongs().size())) {
                            pl->removeSong(pl->getSongs()[songNum - 1]);
                            cout << "Song removed from playlist!" << endl;
                        }
                    }
                    else if (manageChoice == 3) {
                        user->deletePlaylist(pl);
                        cout << "Playlist deleted!" << endl;
                    }
                }
            }
            break;
        }
        case 6: { // Search
            cout << "Enter search query: ";
            string query;
            getline(cin, query);

            vector<Song*> songResults = user->searchSongs(query);
            vector<Playlist*> plResults = user->searchPlaylists(query);

            cout << "\nSearch Results:" << endl;
            cout << "Songs (" << songResults.size() << "):" << endl;
            for (size_t i = 0; i < songResults.size(); i++) {
                cout << i + 1 << ". " << songResults[i]->getTitle() << " by "
                    << songResults[i]->getArtist()->getName() << endl;
            }

            cout << "\nPlaylists (" << plResults.size() << "):" << endl;
            for (size_t i = 0; i < plResults.size(); i++) {
                cout << i + 1 << ". " << plResults[i]->getName() << " by "
                    << plResults[i]->getCreator()->getUsername() << endl;
            }

            if (!songResults.empty()) {
                cout << "\nSelect a song to add to favorites (0 to cancel): ";
                int songChoice;
                cin >> songChoice;
                cin.ignore();

                if (songChoice > 0 && songChoice <= static_cast<int>(songResults.size())) {
                    user->addFavoriteSong(songResults[songChoice - 1]);
                    cout << "Song added to favorites!" << endl;
                }
            }
            break;
        }
        case 7: { // Play Music
            if (!user->getCurrentPlaylist()) {
                cout << "No playlist selected. Please select a playlist first." << endl;
                break;
            }

            Playlist* current = user->getCurrentPlaylist();
            Song* currentSong = user->getCurrentSong();

            if (!currentSong) {
                cout << "No song selected. Starting from first song." << endl;
                currentSong = current->getSongs().empty() ? nullptr : current->getSongs()[0];
                user->setCurrentSong(currentSong);
            }

            if (currentSong) {
                cout << "\nNow Playing: " << currentSong->getTitle() << " by "
                    << currentSong->getArtist()->getName() << endl;

                cout << "\nPlayback Controls:" << endl;
                cout << "1. Next" << endl;
                cout << "2. Previous" << endl;
                cout << "3. Toggle Loop (" << (user->isLoopingEnabled() ? "ON" : "OFF") << ")" << endl;
                cout << "4. Change Playback Mode" << endl;
                cout << "5. Back" << endl;

                int playChoice;
                cin >> playChoice;
                cin.ignore();

                switch (playChoice) {
                case 1: {
                    Song* next = user->getNextSong();
                    if (next) {
                        user->setCurrentSong(next);
                        cout << "Playing next: " << next->getTitle() << endl;
                    }
                    else {
                        cout << "End of playlist reached." << endl;
                    }
                    break;
                }
                case 2: {
                    Song* prev = user->getPreviousSong();
                    if (prev) {
                        user->setCurrentSong(prev);
                        cout << "Playing previous: " << prev->getTitle() << endl;
                    }
                    else {
                        cout << "Beginning of playlist reached." << endl;
                    }
                    break;
                }
                case 3:
                    user->toggleLoop();
                    cout << "Loop " << (user->isLoopingEnabled() ? "enabled" : "disabled") << endl;
                    break;
                case 4: {
                    cout << "Select playback mode:" << endl;
                    cout << "1. Sequential" << endl;
                    cout << "2. Random" << endl;
                    cout << "3. Repeat One" << endl;

                    int modeChoice;
                    cin >> modeChoice;
                    cin.ignore();

                    switch (modeChoice) {
                    case 1: user->setPlaybackMode(PlaybackMode::SEQUENTIAL); break;
                    case 2: user->setPlaybackMode(PlaybackMode::RANDOM); break;
                    case 3: user->setPlaybackMode(PlaybackMode::REPEAT); break;
                    }
                    cout << "Playback mode updated." << endl;
                    break;
                }
                }
            }
            else {
                cout << "No songs in the current playlist." << endl;
            }
            break;
        }
        case 8: // Logout
            return;
        default:
            cout << "Invalid choice. Try again." << endl;
        }
    }
}

void adminMenu(Admin* admin) {
    while (true) {
        admin->displayMenu();
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1: { // Add Song
            cout << "Enter song title: ";
            string title;
            getline(cin, title);

            displayArtists(allArtists);
            cout << "Select artist (or 0 to create new): ";
            int artistChoice;
            cin >> artistChoice;
            cin.ignore();

            Artist* artist = nullptr;
            if (artistChoice == 0) {
                cout << "Enter new artist name: ";
                string artistName;
                getline(cin, artistName);
                admin->createArtist(artistName);
                artist = allArtists.back();
            }
            else if (artistChoice > 0 && artistChoice <= static_cast<int>(allArtists.size())) {
                artist = allArtists[artistChoice - 1];
            }
            else {
                cout << "Invalid choice." << endl;
                break;
            }

            cout << "Enter release year: ";
            int year;
            cin >> year;
            cin.ignore();

            cout << "Enter genre: ";
            string genre;
            getline(cin, genre);

            admin->addSong(title, artist, year, genre);
            cout << "Song added successfully!" << endl;
            break;
        }
        case 2: { // Create Artist
            cout << "Enter artist name: ";
            string name;
            getline(cin, name);
            admin->createArtist(name);
            cout << "Artist created successfully!" << endl;
            break;
        }
        case 3: { // Create Album
            displayArtists(allArtists);
            cout << "Select artist: ";
            int artistChoice;
            cin >> artistChoice;
            cin.ignore();

            if (artistChoice > 0 && artistChoice <= static_cast<int>(allArtists.size())) {
                Artist* artist = allArtists[artistChoice - 1];

                cout << "Enter album name: ";
                string name;
                getline(cin, name);

                admin->createAlbum(artist, name);
                cout << "Album created successfully!" << endl;

                // Add songs to album
                Playlist* album = allPlaylists.back();
                displaySongs(artist->getSongs());
                cout << "Select songs to add to album (0 when done): ";

                while (true) {
                    int songChoice;
                    cin >> songChoice;
                    cin.ignore();

                    if (songChoice == 0) break;
                    if (songChoice > 0 && songChoice <= static_cast<int>(artist->getSongs().size())) {
                        album->addSong(artist->getSongs()[songChoice - 1]);
                    }
                }
            }
            else {
                cout << "Invalid choice." << endl;
            }
            break;
        }
        case 4: // Browse Songs
            displaySongs(allSongs);
            break;
        case 5: // Browse Playlists
            displayPlaylists(allPlaylists);
            break;
        case 6: // Browse Artists
            displayArtists(allArtists);
            break;
        case 7: // Logout
            return;
        default:
            cout << "Invalid choice. Try again." << endl;
        }
    }
}

void loginMenu() {
    while (true) {
        cout << "\nMusic Player System" << endl;
        cout << "1. Login" << endl;
        cout << "2. Register" << endl;
        cout << "3. Exit" << endl;
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1: { // Login
            cout << "Username: ";
            string username;
            getline(cin, username);

            cout << "Password: ";
            string password;
            getline(cin, password);

            // Check admin
            if (admin && admin->authenticate(username, password)) {
                adminMenu(admin);
                break;
            }

            // Check users
            User* loggedInUser = nullptr;
            for (auto& user : allUsers) {
                if (user->authenticate(username, password)) {
                    loggedInUser = user;
                    break;
                }
            }

            if (loggedInUser) {
                userMenu(loggedInUser);
            }
            else {
                cout << "Invalid username or password." << endl;
            }
            break;
        }
        case 2: { // Register
            cout << "Enter username: ";
            string username;
            getline(cin, username);

            // Check if username exists
            bool exists = false;
            for (auto& user : allUsers) {
                if (user->getUsername() == username) {
                    exists = true;
                    break;
                }
            }

            if (exists) {
                cout << "Username already exists." << endl;
                break;
            }

            cout << "Enter password: ";
            string password;
            getline(cin, password);

            allUsers.push_back(new User(username, password));
            cout << "Registration successful! You can now login." << endl;
            break;
        }
        case 3: // Exit
            return;
        default:
            cout << "Invalid choice. Try again." << endl;
        }
    }
}

void initializeSystem() {
    // Create admin
    admin = new Admin("admin", "admin123");

    // Create some sample data
    Artist* artist1 = new Artist("Artist One");
    Artist* artist2 = new Artist("Artist Two");
    allArtists.push_back(artist1);
    allArtists.push_back(artist2);

    admin->addSong("Song One", artist1, 2020, "Pop");
    admin->addSong("Song Two", artist1, 2021, "Rock");
    admin->addSong("Song Three", artist2, 2019, "Jazz");
    admin->addSong("Song Four", artist2, 2022, "Electronic");

    admin->createAlbum(artist1, "First Album");
    admin->createAlbum(artist2, "Debut Album");

    // Add songs to albums
    Playlist* album1 = allPlaylists[0];
    Playlist* album2 = allPlaylists[1];

    album1->addSong(allSongs[0]);
    album1->addSong(allSongs[1]);
    album2->addSong(allSongs[2]);
    album2->addSong(allSongs[3]);
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));
    initializeSystem();
    loginMenu();

    // Cleanup
    delete admin;
    for (auto& user : allUsers) delete user;
    for (auto& song : allSongs) delete song;
    for (auto& playlist : allPlaylists) delete playlist;
    for (auto& artist : allArtists) delete artist;

    return 0;
}