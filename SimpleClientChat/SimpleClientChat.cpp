#define NOMINMAX
#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <limits>


const char Ipv4[] = "127.0.0.1"; //Текущий Ip-адрес сервера
const int port = 1000; //Текущий порт сервера
int erStat;

enum user_events
{
	SEND_MESSAGE_TO_USER, //Отправить сообщение другому пользователю +
	SEND_MESSAGE_TO_CHANNEL, //Отправить сообщение в канал
	CREATE_CHANNEL, //Создать канал +
	CONNECT_TO_CHANNEL, //Подключиться к каналу
	DISCONNECT_FROM_CHANNEL, //Отключиться от канала
	FIND_USER, //Найти пользователя +
	DELETE_CHANNEL, //Удалить канал +
	EVENT_ERROR, // Ошибка +
	FIND_CHANNEL, // Найти канал +
	ADD_TO_CHANNEL_USER, //Добавить в канал пользователя
	EVENT_COMPLETE, // Действие клиента успешно обработано +
	CHECK_ONLINE, //Проверить доступность другого клиента +
	EVENT_ERROR_WITHOUT_DISCONNECT, // Ошибка без отключения пользователя +
	CHECK_MESSAGES, //Проверка входящих сообщений +
	CHECK_MESSAGES_FROM_CHANNELS //Проверка входящих сообщений от пользователей 
};

in_addr getIpv4FromChar(const char* ip)
{
	in_addr ip_to_num;
	inet_pton(AF_INET, ip, &ip_to_num);
	return ip_to_num;
}

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	//Инициализация библиотеки сокетов
	WSAData wsaData;
	int IfItStarts;
	IfItStarts = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (IfItStarts != 0) {
		std::cout << "Ошибка запуска библиотеки сокетов!" << std::endl << "Закрытие сервера...";
		return 1;
	}
	SOCKET client = socket(AF_INET, SOCK_STREAM, NULL);
	if (client == INVALID_SOCKET) {
		std::cout << "Ошибка инициализации прослушивающего сокета!" << std::endl << "Закрытие сервера...";
		closesocket(client);
		WSACleanup();
		return 1;
	}
	//Заполнение структуры, используемой для привязки сокета к адресу
	SOCKADDR_IN serverInfo;
	int sizeofaddr = sizeof(serverInfo);
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_addr = getIpv4FromChar(Ipv4);
	serverInfo.sin_port = htons(port);
	erStat = connect(client, (sockaddr*)&serverInfo, sizeof(serverInfo));
	if (erStat != 0) {
		std::cout << "Ошибка подключения!" << std::endl;
		closesocket(client);
		WSACleanup();
		return 1;
	}
	else
		std::cout << "Вы успешно подключились к серверу!" << std::endl;
	while (true) 
	{
		bool reg_or_log;
		std::cout << "Вход - 0" << std::endl << "Регистрация - 1" << std::endl;
		std::cin >> reg_or_log;
		erStat = send(client, (char*)&reg_or_log, sizeof(bool), NULL);
		if (reg_or_log == 1) {
			std::string log_pass, login, password;
			std::cout << "Введите желаемый логин: ";
			std::cin >> login;
			std::cout << "Введите желаемый пароль: ";
			std::cin >> password;
			if (login.length() < 16) {
				while (login.length() != 16)
				{
					login += ' ';
				}
			}
			if (password.length() < 16) {
				while (password.length() != 16)
				{
					password += ' ';
				}
			}
			log_pass = login + ' ' + password;
			char msg[33];
			bool ok = true;
			bool error = false;
			bool statmsg;
			log_pass.copy(msg, log_pass.length());
			send(client, msg, sizeof(msg), NULL);
			recv(client, (char*)&statmsg, sizeof(bool), NULL);
			if (statmsg == ok) {
				std::cout << "Регистрация успешна!" << std::endl;
				break;
			}
			else if (statmsg == error) {
				std::cout << "Данное имя пользователя уже существует!" << std::endl;
				continue;
			}
		}
		else {
			std::string log_pass, login, password;
			std::cout << "Введите логин: ";
			std::cin >> login;
			std::cout << "Введите пароль: ";
			std::cin >> password;
			if (login.length() < 16) {
				while (login.length() != 16)
				{
					login += ' ';
				}
			}
			if (password.length() < 16) {
				while (password.length() != 16)
				{
					password += ' ';
				}
			}
			log_pass = login + ' ' + password;
			char msg[33];
			bool ok = true;
			bool error = false;
			bool statmsg;
			log_pass.copy(msg, log_pass.length());
			send(client, msg, sizeof(msg), NULL);
			recv(client, (char*)&statmsg, sizeof(bool), NULL);
			if (statmsg == ok) {
				std::cout << "Вы успешно вошли!" << std::endl;
				break;
			}
			else if (statmsg == error) {
				std::cout << "Неправильные имя пользователя и(или) пароль!" << std::endl;
				continue;
			}
		}
	}
	std::cout << "Добро пожаловать в чат!" << std::endl;
	while (true)
	{
		int choose = 0;
		std::cout << "\nВведите - 0, чтобы посмотреть наличие входящих сообщений!" << std::endl << "Введите - 1, чтобы найти пользователя!" << std::endl << "Введите - 2, чтобы проверить онлайн пользователя!" << std::endl << "Введите - 3, чтобы написать пользователю!" << std::endl << "Введите - 4, чтобы выйти!\n" << std::endl;
		std::cin >> choose;
		switch (choose)
		{
		case 1:
		{
			user_events find = FIND_USER;
			send(client, (char*)&find, sizeof(user_events), NULL);
			std::cout << "Введите имя пользователя, которого хотите найти: ";
			std::string username;
			std::cin >> username;
			int msg_size = username.length();
			send(client, (char*)&msg_size, sizeof(int), NULL);
			char* msg = new char[msg_size];
			username.copy(msg, msg_size);
			send(client, msg, msg_size, NULL);
			delete[] msg;
			int answer_size;
			recv(client, (char*)&answer_size, sizeof(int), NULL);
			if (answer_size == 0)
			{
				std::cout << "Пользователи не найдены!" << std::endl;
				break;
			}
			else
			{
				char* answer = new char[answer_size];
				recv(client, answer, answer_size, NULL);
				std::string answer_msg = std::string(answer, answer_size);
				delete[] answer;
				std::cout << "Найденные пользователи: " << answer_msg << std::endl;
				break;
			}
		}
		case 2:
		{
			user_events check = CHECK_ONLINE;
			send(client, (char*)&check, sizeof(user_events), NULL);
			std::cout << "Введите имя пользователя, которого хотите проверить: ";
			std::string username;
			std::cin >> username;
			int msg_size = username.length();
			send(client, (char*)&msg_size, sizeof(int), NULL);
			char* msg = new char[msg_size];
			username.copy(msg, msg_size);
			send(client, msg, msg_size, NULL);
			delete[] msg;
			bool status;
			recv(client, (char*)&status, sizeof(bool), NULL);
			if (status == true)
				std::cout << "Пользователь " << username << " онлайн" << std::endl;
			else

				std::cout << "Пользователь " << username << " оффлайн" << std::endl;
			break;
		}
		case 3:
		{
			user_events send_message_to_user = SEND_MESSAGE_TO_USER;
			send(client, (char*)&send_message_to_user, sizeof(user_events), NULL);
			std::string who, message;
			std::cout << "Кому: ";
			std::cin >> who;
			std::cout << "Сообщение: ";
			std::cin >> message;
			std::string message_form = who+char(30)+message;
			int msg_size = message_form.length();
			send(client, (char*)&msg_size, sizeof(int), NULL);
			char* msg = new char[msg_size];
			message_form.copy(msg, msg_size);
			send(client, msg, msg_size, NULL);
			delete[] msg;
			break;
		}
		case 4:
		{
			closesocket(client);
			WSACleanup();
			std::cout << "Вы отключились от чата!" << std::endl;
			return 0;
		}
		default:
		{
			user_events check_messages = CHECK_MESSAGES;
			send(client, (char*)&check_messages, sizeof(user_events), NULL);
			std::map <std::string,std::vector<std::string>> pool_of_messages;
			int messages_size;
			recv(client, (char*)&messages_size, sizeof(int), NULL);
			if (messages_size != 0)
			{
				char* msg = new char[messages_size];
				recv(client, msg, messages_size, NULL);
				std::string messages = std::string(msg, messages_size);
				delete[] msg;
				while (true)
				{
					if (messages.find(char(31)) != std::string::npos)
					{
						std::string message = messages.substr(0, messages.find(char(31)));
						pool_of_messages[message.substr(0, message.find(char(30)))].push_back(message.substr(message.find(char(30)) + 1));
						messages = messages.substr(messages.find(char(31)) + 1);
					}
					else break;
				}
				for (const auto& message_form : pool_of_messages)
				{
					std::cout << "Сообщения полученные от " << message_form.first << ":" << std::endl;
					for (const std::string& message : message_form.second)
					{
						std::cout << message << std::endl;
					}
				}
			}
			break;
		}
		}
	}
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
