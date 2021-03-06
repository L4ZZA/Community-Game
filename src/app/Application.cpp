﻿#include "Application.h"

#include "states/StateMenu.h"
#include "states/StatePlaying.h"

#include "../util/Log.h"

Application* Application::instance = nullptr;

Application::Application(std::string&& name, const WindowSettings& settings)
	: m_title(std::move(name))
	, m_inputScheme("Controls.json")
	, m_inputManager(&m_inputScheme)
	, m_windowSettings(settings)
{
	instance = this;

	LOG_WARN("Launching window");
	LOG_WARN("------------------------------------");
	if (!settings.fullscreen)
		LOG_WARN("  Resolution: ", settings.width, " x ", settings.height);
	LOG_WARN("  Fullscreen: ", settings.fullscreen);
	LOG_WARN("  VSync: ", settings.vsync);
	LOG_WARN("------------------------------------");

	auto style = settings.fullscreen ? sf::Style::Fullscreen : sf::Style::Default;

	if (settings.fullscreen)
	{
		sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
		m_window.create({ desktop.width, desktop.height }, m_title, style);
	}
	else
		m_window.create({ settings.width, settings.height }, m_title, style);

	m_window.setVerticalSyncEnabled(settings.vsync);

	m_labelView = sf::View(Vec2(static_cast<float>(m_window.getSize().x / 2), static_cast<float>(m_window.getSize().y / 2)), Vec2(static_cast<float>(m_window.getSize().x), static_cast<float>(m_window.getSize().y)));

	m_fpsLabel = new UI::Label(sf::Text("", getResources().fonts.get("SourceCodePro-Regular"), 22), UI::Label::Alignment::RIGHT);
	m_frameTimeLabel = new UI::Label(sf::Text("", getResources().fonts.get("SourceCodePro-Regular"), 22), UI::Label::Alignment::RIGHT);
	
	m_fpsLabel->setPosition(Vec2(m_labelView.getCenter().x + m_labelView.getSize().x / 2 - 20, m_labelView.getCenter().y - m_labelView.getSize().y / 2 + 10));
	m_frameTimeLabel->setPosition(Vec2(m_labelView.getCenter().x + m_labelView.getSize().x / 2 - 20, m_labelView.getCenter().y - m_labelView.getSize().y / 2 + 38));

	pushState(std::make_unique<State::Menu>(this, &m_window));
}

void Application::start()
{
	constexpr static auto UP_TICK = 1000.0f / 60.0f;

	sf::Clock clock;

	float timer = 0.0f;
	float upTimer = float(clock.getElapsedTime().asMilliseconds());

	uint frames = 0;
	uint updates = 0;

	Timestep timestep(static_cast<float>(clock.getElapsedTime().asMilliseconds()));
	BGM.loadMusic();
	BGM.menu.setVolume(3.0f);
	BGM.menu.setLoop(true);
	BGM.play(BGM.menu);

	while (m_window.isOpen())
	{
		m_window.clear();
		sf::Event e;
		while (m_window.pollEvent(e))
		{
			handleEvents(e);
		}
		//Runs 60 times a second
		float now = float(clock.getElapsedTime().asMilliseconds());
		if (now - upTimer > UP_TICK)
		{
			timestep.update(now);
			updates++;
			upTimer += UP_TICK;
			m_states.back()->input();
			m_states.back()->update(timestep);
		}

		//Runs as fast as possible
		frames++;
		sf::Clock frametime;
		m_states.back()->render(m_window);
		m_frameTime = float(frametime.getElapsedTime().asMilliseconds());
		m_frameTimeLabel->setText(std::to_string((int)round(m_frameTime)) + " ms");

		sf::View oldView = m_window.getView();
		m_window.setView(m_labelView);
		m_fpsLabel->render(m_window);
		m_frameTimeLabel->render(m_window);
		m_window.setView(oldView);
		
		// Runs each second
		if (clock.getElapsedTime().asSeconds() - timer > 1.0f)
		{
			timer += 1.0f;
			m_framesPerSecond = frames;
			m_updatesPerSecond = updates;
			m_states.back()->tick();
			m_fpsLabel->setText(std::to_string(m_framesPerSecond)+ " fps");
			LOG_INFO("FPS: ", m_framesPerSecond, ", UPS: ", m_updatesPerSecond);
			frames = 0;
			updates = 0;
		}

		m_window.display();
	}
}

void Application::handleEvents(sf::Event& event)
{
	switch (event.type)
	{
	case sf::Event::Closed:
		m_window.close();
		break;

	case sf::Event::KeyReleased:
		switch (event.key.code)
		{
		case sf::Keyboard::E:
			m_window.close();
			break;

		default:
			break;
		}
		break;

	default:
		break;
	}

	m_states.back()->event(event);
}

void Application::pushState(std::unique_ptr<State::Base> state)
{
	m_states.push_back(std::move(state));
}

void Application::popState()
{
	m_states.pop_back();
}

const WindowSettings& Application::getSettings() const
{
	return m_windowSettings;
}

ResourceHolder& Application::getResources()
{
	return m_resources;
}

sf::RenderWindow& Application::getWindow()
{
	return m_window;
}

void Application::setVSync(bool enabled)
{
	m_windowSettings.vsync = enabled;
	m_window.setVerticalSyncEnabled(enabled);
}

bool Application::inputPressed(std::string action)
{
	return m_inputManager.isInput(action) && m_window.hasFocus();
}

sf::Vector2i Application::mousePosition()
{
	return m_inputManager.mousePosition(m_window);
}