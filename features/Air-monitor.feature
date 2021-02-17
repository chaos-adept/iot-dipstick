Feature: Air Monitor
  As a user i want to know air quality based on sensor metrics

  terms
  Metrics - Air quality metrics
  ya.monitoring | monitoring  - the Yandex Monitoring Service

  Scenario: Metric harvesting
    Given A Device has harvested metrics
    When The Device flush the metrics
    Then The Metrics are stored

  Scenario: The Yandex Monitoring presents metrics
    Given Metrics are stored
    And the Yandex Monitoring dashboard is opened
    When An User is looking for the air quality
    Then The Metrics are displayed

  Scenario: The Yandex sends alerts when air quality is poor
    Given Metrics are stored
    And Alert are configured for metrics
    When Metrics run into alerting values
    Then An User is notified