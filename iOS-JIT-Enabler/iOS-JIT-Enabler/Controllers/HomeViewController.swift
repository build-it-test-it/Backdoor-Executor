import UIKit

class HomeViewController: UIViewController {
    
    // MARK: - Properties
    
    private let apiService: APIService = {
        let baseURL = KeychainManager.shared.getServerURL() ?? "https://your-jit-backend-url.onrender.com"
        return APIService(baseURLString: baseURL)
    }()
    
    private lazy var jitService = JITService(apiService: apiService)
    
    private let recentAppsTableView: UITableView = {
        let tableView = UITableView()
        tableView.register(AppCell.self, forCellReuseIdentifier: AppCell.identifier)
        tableView.translatesAutoresizingMaskIntoConstraints = false
        return tableView
    }()
    
    private let enableJITButton: UIButton = {
        let button = UIButton(type: .system)
        button.setTitle("Enable JIT for App", style: .large)
        button.backgroundColor = .systemBlue
        button.setTitleColor(.white, for: .normal)
        button.layer.cornerRadius = 12
        button.translatesAutoresizingMaskIntoConstraints = false
        return button
    }()
    
    private let sessionHistoryButton: UIButton = {
        let button = UIButton(type: .system)
        button.setTitle("Session History", style: .plain)
        button.translatesAutoresizingMaskIntoConstraints = false
        return button
    }()
    
    private let settingsButton: UIButton = {
        let button = UIButton(type: .system)
        button.setImage(UIImage(systemName: "gear"), for: .normal)
        button.translatesAutoresizingMaskIntoConstraints = false
        return button
    }()
    
    private let recentAppsLabel: UILabel = {
        let label = UILabel()
        label.text = "Recent Apps"
        label.font = UIFont.systemFont(ofSize: 18, weight: .bold)
        label.translatesAutoresizingMaskIntoConstraints = false
        return label
    }()
    
    private var recentApps: [App] = []
    
    // MARK: - Lifecycle Methods
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        setupUI()
        setupTableView()
        setupActions()
        checkRegistration()
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        
        // Refresh recent apps
        loadRecentApps()
    }
    
    // MARK: - UI Setup
    
    private func setupUI() {
        view.backgroundColor = .systemBackground
        title = "JIT Enabler"
        
        // Add subviews
        view.addSubview(enableJITButton)
        view.addSubview(recentAppsLabel)
        view.addSubview(recentAppsTableView)
        view.addSubview(sessionHistoryButton)
        
        // Set up navigation bar
        navigationItem.rightBarButtonItem = UIBarButtonItem(image: UIImage(systemName: "gear"), style: .plain, target: self, action: #selector(settingsButtonTapped))
        
        // Set up constraints
        NSLayoutConstraint.activate([
            // Enable JIT button constraints
            enableJITButton.topAnchor.constraint(equalTo: view.safeAreaLayoutGuide.topAnchor, constant: 20),
            enableJITButton.leadingAnchor.constraint(equalTo: view.leadingAnchor, constant: 20),
            enableJITButton.trailingAnchor.constraint(equalTo: view.trailingAnchor, constant: -20),
            enableJITButton.heightAnchor.constraint(equalToConstant: 50),
            
            // Recent apps label constraints
            recentAppsLabel.topAnchor.constraint(equalTo: enableJITButton.bottomAnchor, constant: 30),
            recentAppsLabel.leadingAnchor.constraint(equalTo: view.leadingAnchor, constant: 20),
            recentAppsLabel.trailingAnchor.constraint(equalTo: view.trailingAnchor, constant: -20),
            
            // Recent apps table view constraints
            recentAppsTableView.topAnchor.constraint(equalTo: recentAppsLabel.bottomAnchor, constant: 10),
            recentAppsTableView.leadingAnchor.constraint(equalTo: view.leadingAnchor),
            recentAppsTableView.trailingAnchor.constraint(equalTo: view.trailingAnchor),
            
            // Session history button constraints
            sessionHistoryButton.topAnchor.constraint(equalTo: recentAppsTableView.bottomAnchor, constant: 10),
            sessionHistoryButton.centerXAnchor.constraint(equalTo: view.centerXAnchor),
            sessionHistoryButton.bottomAnchor.constraint(equalTo: view.safeAreaLayoutGuide.bottomAnchor, constant: -20),
            
            // Make the table view fill the space between the label and the session history button
            recentAppsTableView.bottomAnchor.constraint(equalTo: sessionHistoryButton.topAnchor, constant: -10)
        ])
    }
    
    private func setupTableView() {
        recentAppsTableView.delegate = self
        recentAppsTableView.dataSource = self
    }
    
    private func setupActions() {
        enableJITButton.addTarget(self, action: #selector(enableJITButtonTapped), for: .touchUpInside)
        sessionHistoryButton.addTarget(self, action: #selector(sessionHistoryButtonTapped), for: .touchUpInside)
    }
    
    // MARK: - Data Loading
    
    private func loadRecentApps() {
        recentApps = AppManager.shared.getRecentApps()
        
        if recentApps.isEmpty {
            recentAppsLabel.text = "No Recent Apps"
        } else {
            recentAppsLabel.text = "Recent Apps"
        }
        
        recentAppsTableView.reloadData()
    }
    
    // MARK: - Registration
    
    private func checkRegistration() {
        if KeychainManager.shared.getAuthToken() == nil {
            // No auth token, need to register
            registerDevice()
        }
    }
    
    private func registerDevice() {
        let loadingIndicator = showLoadingIndicator()
        
        jitService.registerDevice { [weak self] result in
            guard let self = self else { return }
            
            self.hideLoadingIndicator(indicator: loadingIndicator)
            
            switch result {
            case .success(let message):
                self.showAlert(title: "Registration Successful", message: message)
            case .failure(let error):
                self.showError(error)
                
                // If server URL is not set, prompt for it
                if KeychainManager.shared.getServerURL() == nil {
                    self.promptForServerURL()
                }
            }
        }
    }
    
    private func promptForServerURL() {
        showTextInputAlert(
            title: "Server URL",
            message: "Please enter the JIT backend server URL",
            placeholder: "https://your-jit-backend-url.onrender.com"
        ) { [weak self] url in
            guard let self = self, let url = url, !url.isEmpty else { return }
            
            KeychainManager.shared.saveServerURL(url)
            
            // Recreate API service with new URL
            let apiService = APIService(baseURLString: url)
            self.jitService = JITService(apiService: apiService)
            
            // Try registration again
            self.registerDevice()
        }
    }
    
    // MARK: - Actions
    
    @objc private func enableJITButtonTapped() {
        let appListVC = AppListViewController(jitService: jitService)
        navigationController?.pushViewController(appListVC, animated: true)
    }
    
    @objc private func sessionHistoryButtonTapped() {
        let sessionHistoryVC = SessionHistoryViewController(jitService: jitService)
        navigationController?.pushViewController(sessionHistoryVC, animated: true)
    }
    
    @objc private func settingsButtonTapped() {
        let settingsVC = SettingsViewController(jitService: jitService)
        navigationController?.pushViewController(settingsVC, animated: true)
    }
}

// MARK: - UITableViewDelegate, UITableViewDataSource

extension HomeViewController: UITableViewDelegate, UITableViewDataSource {
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return recentApps.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        guard let cell = tableView.dequeueReusableCell(withIdentifier: AppCell.identifier, for: indexPath) as? AppCell else {
            return UITableViewCell()
        }
        
        let app = recentApps[indexPath.row]
        cell.configure(with: app)
        
        return cell
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        tableView.deselectRow(at: indexPath, animated: true)
        
        let app = recentApps[indexPath.row]
        enableJITForApp(app)
    }
    
    func tableView(_ tableView: UITableView, heightForRowAt indexPath: IndexPath) -> CGFloat {
        return 70
    }
    
    // MARK: - JIT Enablement
    
    private func enableJITForApp(_ app: App) {
        let loadingIndicator = showLoadingIndicator()
        
        jitService.enableJIT(for: app) { [weak self] result in
            guard let self = self else { return }
            
            self.hideLoadingIndicator(indicator: loadingIndicator)
            
            switch result {
            case .success(let response):
                // Save app to recent apps
                AppManager.shared.saveToRecentApps(app)
                
                // Show success message
                self.showAlert(title: "JIT Enabled", message: response.message) {
                    // Launch the app
                    if !self.jitService.launchApp(withBundleId: app.bundleId) {
                        self.showAlert(title: "Launch Failed", message: "Could not launch the app. Please open it manually.")
                    }
                }
                
            case .failure(let error):
                self.showError(error)
            }
        }
    }
}