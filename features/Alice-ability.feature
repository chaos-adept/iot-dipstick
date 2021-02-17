Feature: Alice interactions
  As a user i want to know the current air quality or control the light of the air monitor devices

  Terms:
  the Alice ability - the Extension which allows interact with Yandex Alice Voice assistant
  the Air metrics - air quality metrics from devices

  Scenario: Alice says air quality
    Given Yandex Monitoring has got the Air metrics
    When the Alice ability is activated
    Then Alice greetings with air quality report

  Scenario: Alice controls Air monitor light
    Given Alice ability is activated
    When an User say the light control commands
    Then the Air monitor device execute commands